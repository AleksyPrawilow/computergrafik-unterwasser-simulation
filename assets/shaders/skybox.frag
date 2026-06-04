#version 410 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float time;

void main()
{
    vec4 baseColor = texture(skybox, TexCoords);

    vec4 waterColor = vec4(0.0, 0.05, 0.15, 1.0);
    FragColor = mix(baseColor, waterColor, 0.2);
}