#version 330 core

struct Material
{
    
    sampler2D diffuse;
    sampler2D emission;
    sampler2D specular;
    float shininess;

};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 fragPosition;

out vec4 fragColor;

uniform vec3 viewPosition;



uniform Material material;
uniform Light light;

void main()
{
    // ambient light
    vec3 ambientLight = light.ambient * vec3(texture(material.diffuse, TexCoords));

    //  diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - fragPosition);
    float diff = max( dot(lightDir,norm), 0 );
    vec3 diffuseLight = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));

    vec3 emissionLight = texture(material.emission, TexCoords).rgb ;

    // specular light
    vec3 viewDir = normalize(viewPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularLight = light.specular * spec * vec3(texture(material.specular, TexCoords));  


    // result
    vec3 result = ambientLight + diffuseLight + specularLight + emissionLight;
    fragColor = vec4(result ,1.0);
}
