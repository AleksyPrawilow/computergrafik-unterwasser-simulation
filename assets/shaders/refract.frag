#version 410 core
out vec4 FragColor;

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D normalMap;
uniform float time;

void main() {
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 I = -viewDir;

    vec2 uvScroll = texCoord * 3.0 + vec2(0.0, -time * 0.05);

    vec3 tangentNormal = texture(normalMap, uvScroll).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);

    tangentNormal.xy *= 0.04;
    vec3 N = normalize(TBN * tangentNormal);

    vec3 R = reflect(I, N);
    vec3 reflectionColor = texture(skybox, R).rgb;

    if (cameraPos.y < 0.0) {
        vec3 waterColor = vec3(0.0, 0.05, 0.15);
        float reflectFog = clamp(smoothstep(-0.2, 0.8, R.y), 0.0, 1.0);
        reflectionColor = mix(waterColor, reflectionColor, reflectFog * 0.35);
    }

    float F0 = 0.02;

    float Fresnel = F0 + (1.0 - F0) * pow(1.0 - max(dot(N, viewDir), 0.0), 8.0);

    if (viewDir.y > 0.0) {
        float lookDownFactor = clamp(1.0 - viewDir.y * 1.5, 0.0, 1.0);
        Fresnel *= lookDownFactor;
    }
    vec3 finalColor = reflectionColor;

    vec3 glassTint = vec3(0.01, 0.03, 0.045);
    finalColor += glassTint;

    float dist = length(cameraPos - worldPos);
    float fogDensity = 0.035;
    float fogFactor = clamp(exp(-dist * fogDensity), 0.0, 1.0);
    vec3 waterFogColor = vec3(0.0, 0.05, 0.15);
    vec3 foggedColor = mix(waterFogColor, finalColor, fogFactor);

    float alpha = mix(0.80, 0.08, 1.0 - Fresnel);

    FragColor = vec4(foggedColor, alpha);
}