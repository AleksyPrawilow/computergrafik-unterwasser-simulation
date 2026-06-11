#include "pbr_lighting.glsl"

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

out vec4 outColor;

uniform sampler2D colorTexture;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D emissionMap;
uniform sampler2D opacityMap;
uniform vec3 cameraPos;
uniform float time;

float calculateCaustics(vec2 xz, float t) {
    vec2 p = mod(xz * 0.12, 6.28) - 250.0;
    vec2 i = vec2(p);
    float c = 1.0;
    float inten = 0.005;

    for (int n = 0; n < 3; n++) {
        float tOffset = t * (1.0 - (3.5 / float(n + 1)));
        i = p + vec2(cos(tOffset - i.x) + sin(tOffset + i.y), sin(tOffset - i.y) + cos(tOffset + i.x));
        c += 1.0 / length(vec2(p.x / (sin(i.x + tOffset) / inten), p.y / (cos(i.y + tOffset) / inten)));
    }
    c /= 3.0;
    c = 1.17 - pow(c, 1.4);

    return pow(max(c, 0.0), 8.0) * 0.5;
}

vec4 calculatePBR() {
    vec3 albedo = texture(colorTexture, texCoord).rgb;
    float roughness = texture(roughnessMap, texCoord).g;
    float metallic  = texture(metallicMap, texCoord).b;
    vec3 emission = texture(emissionMap, texCoord).rgb;
    float alpha = texture(opacityMap, texCoord).r;

    vec3 tangentNormal = texture(normalMap, texCoord).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);
    vec3 N = normalize(TBN * tangentNormal);

    vec3 V = normalize(cameraPos - worldPos);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float ambientScale = mix(0.01, 0.12, smoothstep(-2.0, 0.0, cameraPos.y));

    float depthFactor = 1.0;
    if (worldPos.y < 0.0) {
        depthFactor = clamp(exp(worldPos.y * 0.02), 0.0, 1.0);
    }

    vec3 ambient = vec3(ambientScale) * albedo * depthFactor;

    vec3 Lo = vec3(0.0);

    vec3 sunDir = normalize(vec3(0.1, 1.0, 0.15));
    vec3 sunColor = vec3(1.0, 0.95, 0.9);
    float sunIntensity = 1.0;

    vec3 sunRadiance = sunColor * sunIntensity * depthFactor;

    Lo += CalculateCookTorrance(N, V, sunDir, sunRadiance, albedo, roughness, metallic, F0, worldPos);

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
        vec3 L = normalize(pointLights[i].position - worldPos);
        float distance = length(pointLights[i].position - worldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

        Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, worldPos);
    }

    for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
        vec3 L = normalize(spotLights[i].position - worldPos);

        float theta = dot(-L, normalize(spotLights[i].direction));
        float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
        float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        float distance = length(spotLights[i].position - worldPos);
        float attenuation = 1.0 / (distance * distance + 0.5);

        float absorption = exp(-distance * 0.01);

        vec3 radiance = spotLights[i].color * spotLights[i].intensity * attenuation * intensity * absorption;

        Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, worldPos);
    }

    vec3 color = ambient + Lo;

    color += emission * 3.0;

    if (worldPos.y < 0.0) {
        float causticIntensity = calculateCaustics(worldPos.xz, time);
        vec3 causticColor = vec3(0.5, 0.85, 1.0) * causticIntensity * 1.5;

        float projectionMask = clamp(N.y, 0.0, 1.0);
        causticColor *= depthFactor * projectionMask;
        color += causticColor * albedo;
    }

    if (cameraPos.y < 0.0) {
        vec3 absorptionFilter = vec3(0.35, 0.75, 1.0);
        color *= absorptionFilter;
    }

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    if (cameraPos.y < 0.0) {
        float dist = length(cameraPos - worldPos);

        float currentDrift = sin(worldPos.x * 0.08 + time * 0.3)
        * cos(worldPos.z * 0.08 - time * 0.2)
        * sin(worldPos.y * 0.04);

        float baseFogDensity = 0.035;
        float dynamicDensity = baseFogDensity + (currentDrift * 0.008);
        float fogFactor = clamp(exp(-dist * dynamicDensity), 0.0, 1.0);

        vec3 surfaceFogColor = vec3(0.02, 0.22, 0.28);
        vec3 deepAbyssalColor = vec3(0.002, 0.015, 0.06);
        float depthBlend = clamp((worldPos.y + 45.0) / 45.0, 0.0, 1.0);
        vec3 baseWaterColor = mix(deepAbyssalColor, surfaceFogColor, depthBlend);

        vec3 sunDirection = normalize(vec3(0.1, 1.0, 0.15));
        float viewSunAngle = max(dot(-V, sunDirection), 0.0);
        vec3 sunHazeColor = vec3(0.4, 0.75, 0.9) * pow(viewSunAngle, 6.0) * 0.3;

        vec3 finalFogColor = baseWaterColor + sunHazeColor;

        for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
            if (spotLights[i].intensity > 0.0) {
                vec3 L = normalize(spotLights[i].position - worldPos);
                float theta = dot(-L, normalize(spotLights[i].direction));
                float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
                float spotIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

                float distanceToLight = length(spotLights[i].position - worldPos);
                float attenuation = 1.0 / (distanceToLight * distanceToLight + 0.5);
                float absorption = exp(-distanceToLight * 0.01);

                vec3 fogGlow = spotLights[i].color * spotLights[i].intensity * attenuation * spotIntensity * 0.15 * absorption;
                finalFogColor += fogGlow;
            }
        }

        color = mix(finalFogColor, color, fogFactor);
    }

    return vec4(color, alpha);
}