#version 410 core
out vec4 FragColor;

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

uniform vec3 cameraPos;
uniform float time;
uniform sampler2D normalMap;

void main() {
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 lightDir = normalize(vec3(0.3, 1.0, 0.4));

    vec2 uv1 = texCoord * 15.0 + vec2(time * 0.02, time * 0.01);
    vec2 uv2 = texCoord * 15.0 + vec2(time * -0.015, time * 0.02);

    vec3 norm1 = texture(normalMap, uv1).rgb * 2.0 - 1.0;
    vec3 norm2 = texture(normalMap, uv2).rgb * 2.0 - 1.0;

    vec3 blendedNormal = normalize(norm1 + norm2);
    vec3 normal = normalize(TBN * blendedNormal);

    vec3 surfaceColor = vec3(0.0, 0.35, 0.5);

    vec3 ambient = surfaceColor * 0.4;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = surfaceColor * diff * 0.6;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specFactor = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
    vec3 specular = vec3(0.9, 0.95, 1.0) * specFactor * 0.8;

    vec3 finalColor = ambient + diffuse + specular;

    vec3 fogColor;
    if (cameraPos.y < 0.0) {
        fogColor = vec3(0.0, 0.05, 0.15);
    } else {
        fogColor = vec3(0.4, 0.6, 0.9);
    }

    float dist = length(cameraPos - worldPos);
    float fogDensity = 0.035;
    float fogFactor = clamp(exp(-dist * fogDensity), 0.0, 1.0);

    vec3 foggedColor = mix(fogColor, finalColor, fogFactor);
    float finalAlpha = mix(1.0, 0.6, fogFactor);

    FragColor = vec4(foggedColor, finalAlpha);
}