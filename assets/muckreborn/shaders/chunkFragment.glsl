#version 330 core

out vec4 FragColor;

struct Material 
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
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

    bool isActive;
};

struct SpotLight 
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;   
    
    bool isActive;
};

#define MAX_LIGHTS 4

in vec3 ourColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 normal;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];
uniform Material material;
uniform sampler2D shadowMap;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{
    vec4 color1 = vec4(0.812, 0.792, 0.298, 1.0f);
    vec4 color2 = vec4(0.518, 0.941, 0.192, 1.0);

    vec4 color3 = vec4(1, 0.996, 0.843, 1.0f);
    vec4 color4 = vec4(0.518, 0.941, 0.192, 1.0);

    float factor1 = clamp(FragPos.y / 0.08, 0.0, 1.0);
    float factor2 = clamp(FragPos.y / 0.08, 0.0, 1.0);

    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for(int p = 0; p < MAX_LIGHTS; p++)
    {
        if(pointLights[p].isActive)
            result += CalcPointLight(pointLights[p], norm, FragPos, viewDir);   
    }
         
    for(int s = 0; s < MAX_LIGHTS; s++)
    {
        if(spotLights[s].isActive)
            result += CalcSpotLight(spotLights[s], norm, FragPos, viewDir);    
    }

    vec3 litColor1 = result * color1.rgb;
    vec3 litColor2 = result * color2.rgb;

    vec3 litColor3 = result * color3.rgb;
    vec3 litColor4 = result * color4.rgb;

    litColor1 = pow(litColor1, vec3(1.0/2.2));
    litColor2 = pow(litColor2, vec3(1.0/2.2));
    litColor3 = pow(litColor3, vec3(1.0/2.2));
    litColor4 = pow(litColor4, vec3(1.0/2.2));

    vec3 gradientResult = mix(litColor1, litColor2, factor1) + mix(litColor3, litColor4, factor2);
    FragColor = vec4(gradientResult, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(pointLights[0].position - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }

    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    float shadow = ShadowCalculation(FragPosLightSpace);   

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * vec3(1.0, 1.0, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    float shadow = ShadowCalculation(FragPosLightSpace);    

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * vec3(1.0, 1.0, 1.0);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}