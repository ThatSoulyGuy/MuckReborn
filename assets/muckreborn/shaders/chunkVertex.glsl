#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 FragPos;
out vec4 FragPosLightSpace;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    ourColor = aColor;
    TexCoord = aTexCoord;
    normal = aNormal;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}