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

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    
    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


#define NR_POINT_LGIHTS 4
uniform PointLight pointLights[NR_POINT_LGIHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;

in vec2 TexCoords;
in vec3 Normal;
in vec3 fragPosition;

out vec4 fragColor;

uniform vec3 viewPosition;



uniform Material material;
uniform Light light;

//Functions

vec3 CalcDirLight(DirLight light, vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viweDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir  = normalize(viewPosition - fragPosition);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < NR_POINT_LGIHTS; i++)
    {
        result += CalcPointLight(pointLights[i], norm, fragPosition, viewDir);

    }
    result += CalcSpotLight(spotLight, norm, fragPosition, viewDir);

    fragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal,vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal,lightDir),0.0);

    //specular shading
    vec3 reflectDir = reflect(-lightDir , normal);
    float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);

    // result
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));


    return ambient + diffuse + specular;
    
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / ( light.constant +  
                                light.linear * distance  +
                                light.quadratic * (distance * distance)
                                );

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);
    
    // specular shading
    vec3 reflectDir = reflect( -lightDir , normal);
    float ref = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);

    vec3 ambient = light.ambient * attenuation * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * attenuation * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * ref * attenuation  * vec3(texture(material.specular, TexCoords));

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant +
                               light.linear * distance +
                               light.quadratic * (distance * distance)
                               );

    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // spotlight  intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // specular
    vec3 reflectDir = reflect( -lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);


    vec3 ambient = light.ambient * attenuation * intensity * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * attenuation * intensity * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * attenuation * intensity * vec3(texture(material.specular, TexCoords));

    return ambient + diffuse + specular;

}


