#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 outPosition;

uniform sampler2D texture_diffuse1;

void main()
{
    vec4 color1 = vec4(0.91, 0.898, 0.6, 1.0f);
    vec4 color2 = vec4(0.518, 0.941, 0.192, 1.0);

    vec4 color3 = vec4(1, 0.996, 0.843, 1.0f);
    vec4 color4 = vec4(0.518, 0.941, 0.192, 1.0);

    float factor1 = clamp(outPosition.y / 0.08, 0.0, 1.0);
    float factor2 = clamp(outPosition.y / 0.08, 0.0, 1.0);

    float aoFactor = 1.0 - smoothstep(0.0, 0.1, outPosition.y);
    vec4 aoColor = vec4(aoFactor, aoFactor, aoFactor, 1.0);

    FragColor *= aoColor;
    FragColor = mix(color1, color2, factor1);
}