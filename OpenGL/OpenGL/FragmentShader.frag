#version 330 core
in vec3 Normal;
in vec3 fragPosition;

out vec4 fragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main()
{
    // ambient light
    float ambientStrength = 0.1;
    vec3 ambientLight = ambientStrength * lightColor;

    //  diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - fragPosition);
    
    float diffuseStrength = max( dot(lightDir,norm), 0 );
    vec3 diffuseLight = diffuseStrength * lightColor;

    // specular light
    float specularStrength = 0.5;

    vec3 viewDir = normalize(viewPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);

    float shininess = 32;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specularLight = specularStrength * spec * lightColor;  


    // result
    vec4 result = vec4((ambientLight + diffuseLight + specularLight) * objectColor,1.0);

    fragColor = result;
}
