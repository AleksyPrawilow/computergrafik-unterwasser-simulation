// assets/shaders/common_pbr.glsl
#include "pbr_lighting.glsl"

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

out vec4 outColor;

uniform sampler2D colorTexture;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform vec3 cameraPos;

vec3 calculatePBR() {
    vec3 albedo = texture(colorTexture, texCoord).rgb;
    float roughness = texture(roughnessMap, texCoord).g;
    float metallic  = texture(metallicMap, texCoord).b;

    // Normals
    vec3 tangentNormal = texture(normalMap, texCoord).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);
    vec3 N = normalize(TBN * tangentNormal);

    vec3 V = normalize(cameraPos - worldPos);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // --- 1. DYNAMIC AMBIENT (Brighter on surface, dark underwater) ---
    // Smoothly scale ambient base factor between 0.01 (abyss) and 0.12 (bright sky)
    float ambientScale = mix(0.01, 0.12, smoothstep(-2.0, 0.0, cameraPos.y));

    // Calculate depth-based light absorption (if fragment itself is below water)
    float depthFactor = 1.0;
    if (worldPos.y < 0.0) {
        depthFactor = clamp(exp(worldPos.y * 0.02), 0.0, 1.0);
    }

    vec3 ambient = vec3(ambientScale) * albedo * depthFactor;

    // Accumulated outgoing light
    vec3 Lo = vec3(0.0);

    // --- 2. DEFAULT SUN LIGHT (Directional PBR Light) ---
    vec3 sunDir = normalize(vec3(0.3, 1.0, 0.4)); // Shines down from the sky
    vec3 sunColor = vec3(1.0, 0.95, 0.9);         // Warm, natural sunlight
    float sunIntensity = 1.0;                     // Brightness of the sun

    // The sun's light is absorbed by the water depth as well!
    vec3 sunRadiance = sunColor * sunIntensity * depthFactor;

    Lo += CalculateCookTorrance(N, V, sunDir, sunRadiance, albedo, roughness, metallic, F0, worldPos);

    // A. ACCUMULATE POINT LIGHTS (glowing creatures, etc.)
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
        vec3 L = normalize(pointLights[i].position - worldPos);
        float distance = length(pointLights[i].position - worldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

        Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, worldPos);
    }

    // B. ACCUMULATE SPOTLIGHTS (Flashlights)
    for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
        vec3 L = normalize(spotLights[i].position - worldPos);

        // Spotlight cone angle check
        float theta = dot(-L, normalize(spotLights[i].direction));
        float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
        float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        float distance = length(spotLights[i].position - worldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = spotLights[i].color * spotLights[i].intensity * attenuation * intensity;

        Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, worldPos);
    }

    // Combine
    vec3 color = ambient + Lo;

    // --- 1. UNDERWATER COLOR ABSORPTION FILTER ---
    if (cameraPos.y < 0.0) {
        // Red is heavily absorbed (0.35), Green is slightly absorbed (0.75), Blue passes fully (1.0)
        vec3 absorptionFilter = vec3(0.35, 0.75, 1.0);

        color *= absorptionFilter;
    }
    // ---------------------------------------------

    // 2. Reinhard Tonemapping & Gamma Correction (Crucial for PBR)
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    // --- 3. EXPONENTIAL UNDERWATER DISTANCE FOG ---
    if (cameraPos.y < 0.0) {
        float dist = length(cameraPos - worldPos);
        float fogDensity = 0.035;
        float fogFactor = clamp(exp(-dist * fogDensity), 0.0, 1.0);

        vec3 waterFogColor = vec3(0.0, 0.05, 0.15); // Matches glClearColor
        color = mix(waterFogColor, color, fogFactor);
    }

    return color;
}