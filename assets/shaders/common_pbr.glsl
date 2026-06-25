// assets/shaders/common_pbr.glsl
#include "pbr_lighting.glsl"

layout (std140) uniform GlobalEnvironment {
    vec4 u_sunDirection;
    vec4 u_sunColor;
    vec4 u_ambientColor;
    vec4 u_fogColor;
    vec4 u_heightFogColor;
    vec4 u_causticsColor;
    vec4 u_cameraPos;

    float u_sunEnergy;
    float u_ambientEnergy;
    float u_baseFogDensity;
    float u_heightFogMin;

    float u_heightFogMax;
    float u_causticsScale;
    float u_causticsIntensity;
    float u_depthDimmingCoefficient;

    float u_time;
    float u_bloomThreshold;
    float u_bloomIntensity;

    int u_fogEnabled;
    int u_heightFogEnabled;
    int u_causticsEnabled;
    int u_depthDimmingEnabled;
    int u_bloomEnabled;

    mat4 u_lightSpaceMatrix;
};

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 bloomColor;

uniform sampler2D colorTexture;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D emissionMap;
uniform sampler2D opacityMap;

uniform float alphaCutoff = 0.0;
uniform float u_bloomStrength = 0.0;
uniform float u_emissionPulse = 1.0;
uniform sampler2DShadow u_shadowMap;

float calculateShadow(vec3 wp) {
    vec4 lsPos = u_lightSpaceMatrix * vec4(wp, 1.0);
    vec3 proj = lsPos.xyz / lsPos.w * 0.5 + 0.5;
    if (proj.z > 1.0) return 1.0;
    float bias = 0.003;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            shadow += texture(u_shadowMap, vec3(proj.xy + vec2(x, y) * texelSize, proj.z - bias));
        }
    }
    return shadow / 9.0;
}

// --- PROCEDURAL CAUSTICS GENERATOR ---
float calculateCaustics(vec2 xz, float t, float scale) {
    vec2 p = mod(xz * scale, 6.28) - 250.0;
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
    // Sample Textures
    vec3 albedo = texture(colorTexture, texCoord).rgb;
    float roughness = texture(roughnessMap, texCoord).g;
    float metallic  = texture(metallicMap, texCoord).b;
    vec3 emission = texture(emissionMap, texCoord).rgb;
    float alpha = texture(opacityMap, texCoord).r;

    if (alpha < alphaCutoff) {
        discard;
    }

    // Normals
    vec3 tangentNormal = texture(normalMap, texCoord).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);
    vec3 N = normalize(TBN * tangentNormal);

    vec3 V = normalize(u_cameraPos.xyz - worldPos);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // --- 1. DYNAMIC AMBIENT ---
    float ambientScale = u_ambientEnergy;
    if (u_heightFogEnabled != 0) { // FIX: Changed int check to boolean comparison
       ambientScale = mix(u_ambientEnergy * 0.1, u_ambientEnergy, smoothstep(u_heightFogMin, u_heightFogMax, u_cameraPos.y));
    }

    float depthFactor = 1.0;
    if (u_heightFogEnabled != 0 && worldPos.y < u_heightFogMax && u_depthDimmingEnabled != 0) { // FIX: Changed int checks
        depthFactor = clamp(exp((worldPos.y - u_heightFogMax) * u_depthDimmingCoefficient), 0.01, 1.0);
    }

    // FIX: Extracted .xyz from vec4 ambient color
    // --- 2. DEFAULT SUN LIGHT ---
    float shadow = calculateShadow(worldPos);
    float shadowAmbientFactor = mix(0.4, 1.0, shadow);
    vec3 ambient = u_ambientColor.xyz * ambientScale * albedo * depthFactor * shadowAmbientFactor;

    // Accumulated outgoing light
    vec3 Lo = vec3(0.0);

    vec3 sunRadiance = u_sunColor.xyz * u_sunEnergy * depthFactor * shadow;

    Lo += CalculateCookTorrance(N, V, u_sunDirection.xyz, sunRadiance, albedo, roughness, metallic, F0, worldPos);

    // Point lights
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
        vec3 L = normalize(pointLights[i].position - worldPos);
        float distance = length(pointLights[i].position - worldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

        Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, worldPos);
    }

    // Spotlights (headlights)
    for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
        vec3 L = normalize(spotLights[i].position - worldPos);

        float theta = dot(-L, normalize(spotLights[i].direction));
        float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
        float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        float distance = length(spotLights[i].position - worldPos);
        float attenuation = 1.0 / (distance * distance + 0.5);

        float absorption = exp(-distance * 0.15);

        vec3 radiance = spotLights[i].color * spotLights[i].intensity * attenuation * intensity * absorption;

        Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, worldPos);
    }

    // Combine standard PBR lighting
    vec3 color = ambient + Lo;

    // Add PBR Emission
    color += emission * 3.0 * u_emissionPulse;

    // Projected Caustics
    if (u_causticsEnabled != 0 && u_heightFogEnabled != 0 && worldPos.y < u_heightFogMax) { // FIX: Changed int checks
        float causticIntensity = calculateCaustics(worldPos.xz, u_time, u_causticsScale);
        // FIX: Extracted .xyz from vec4 caustics color
        vec3 causticColor = u_causticsColor.xyz * causticIntensity * u_causticsIntensity;
        float projectionMask = clamp(N.y, 0.0, 1.0);

        causticColor *= depthFactor * projectionMask;
        color += causticColor * albedo;
    }

    // --- 3. UNDERWATER COLOR ABSORPTION FILTER (In Linear Space) ---
    if (u_heightFogEnabled != 0 && u_cameraPos.y < u_heightFogMax) { // FIX: Changed int check
       vec3 absorptionFilter = vec3(0.35, 0.75, 1.0);
       color *= absorptionFilter;
    }

    // --- 4. TONEMAPPING & GAMMA CORRECTION FOR OBJECT COLOR ---
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    if (u_fogEnabled != 0 && (worldPos.y < u_heightFogMax || u_cameraPos.y < u_heightFogMax)) {
        float dist = length(u_cameraPos.xyz - worldPos);

        float currentDrift = sin(worldPos.x * 0.08 + u_time * 0.3)
        * cos(worldPos.z * 0.08 - u_time * 0.2)
        * sin(worldPos.y * 0.04);

        float baseFogDensity = u_baseFogDensity;
        float dynamicDensity = baseFogDensity + (currentDrift * 0.008);
        float fogFactor = clamp(exp(-dist * dynamicDensity), 0.0, 1.0);

        if (u_cameraPos.y >= u_heightFogMax && worldPos.y < u_heightFogMax && u_heightFogEnabled != 0) {
            float h = u_heightFogMax - worldPos.y;
            float waterDist = h / max(abs(V.y), 0.001);

            float waterFogDensity = u_baseFogDensity;
            float waterFogFactor = clamp(exp(-waterDist * waterFogDensity), 0.0, 1.0);

            vec3 waterFogColor = u_fogColor.xyz;
            if (u_depthDimmingEnabled != 0) {
                float depthFactor = clamp(exp((worldPos.y - u_heightFogMax) * u_depthDimmingCoefficient), 0.01, 1.0);
                waterFogColor *= depthFactor;
            }

            color = mix(waterFogColor, color, waterFogFactor);
        }

        vec3 baseWaterColor;
        if (u_cameraPos.y < u_heightFogMax) {
            float depthBlend = clamp((-V.y - (-0.2)) / 1.0, 0.0, 1.0);
            baseWaterColor = mix(u_fogColor.xyz, u_heightFogColor.xyz, depthBlend);
        } else {
            baseWaterColor = u_fogColor.xyz;
        }

        vec3 sunDirection = normalize(u_sunDirection.xyz);
        float viewSunAngle = max(dot(-V, sunDirection), 0.0);
        vec3 sunHazeColor = u_sunColor.xyz * pow(viewSunAngle, 6.0) * 0.3;

        vec3 finalFogColor = baseWaterColor + sunHazeColor;

        for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
            if (spotLights[i].intensity > 0.0) {
                vec3 L = normalize(spotLights[i].position - worldPos);
                float theta = dot(-L, normalize(spotLights[i].direction));
                float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
                float spotIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

                float distanceToLight = length(spotLights[i].position - worldPos);
                float attenuation = 1.0 / (distanceToLight * distanceToLight + 0.5);
                float absorption = exp(-distanceToLight * 0.15);

                vec3 fogGlow = spotLights[i].color * spotLights[i].intensity * attenuation * spotIntensity * 0.15 * absorption;
                finalFogColor += fogGlow;
            }
        }

        if (u_depthDimmingEnabled != 0 && u_cameraPos.y < u_heightFogMax) {
            float cameraDepthFactor = clamp(exp(u_cameraPos.y * u_depthDimmingCoefficient), 0.0, 1.0);
            finalFogColor *= cameraDepthFactor;
        }

        finalFogColor = finalFogColor / (finalFogColor + vec3(1.0));
        finalFogColor = pow(finalFogColor, vec3(1.0 / 2.2));

        color = mix(finalFogColor, color, fogFactor);
    }

    return vec4(color, alpha);
}