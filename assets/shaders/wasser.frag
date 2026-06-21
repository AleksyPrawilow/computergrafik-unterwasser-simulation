#version 410 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 bloomColor;

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
};

in vec3 worldPos;
in vec3 worldNormal;
in vec2 texCoord;
in mat3 TBN;

uniform float u_bloomStrength = 0.0;

// Textures
uniform sampler2D normalMap;
uniform samplerCube skybox;

// Universal Environment Uniforms

// GGX Microfacet Distribution for Specular Highlights
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265 * denom * denom;

    return num / max(denom, 0.0000001);
}

void main() {
    vec3 viewDir = normalize(u_cameraPos.xyz - worldPos);
    vec3 lightDir = u_sunDirection.xyz;

    // 1. DYNAMIC DUAL-LAYER NORMAL SCROLLING
    // We scroll two layers of normal maps in different directions to simulate detailed ripples
    vec2 uv1 = texCoord * 12.0 + vec2(u_time * 0.015, u_time * 0.008);
    vec2 uv2 = texCoord * 24.0 - vec2(u_time * 0.010, u_time * 0.020);

    vec3 n1 = texture(normalMap, uv1).rgb * 2.0 - 1.0;
    vec3 n2 = texture(normalMap, uv2).rgb * 2.0 - 1.0;

    // Combine and blend the normal coordinates
    vec3 tangentNormal = normalize(n1 + n2);

    // Transform tangent normal to World Space using the TBN matrix from the vertex shader
    vec3 normal = normalize(TBN * tangentNormal);

    // 2. FRESNEL REFLECTION APPROXIMATION
    // Water has a base reflectance of ~2% (F0 = 0.02) when looking straight down
    float F0 = 0.02;
    float fresnel = F0 + (1.0 - F0) * pow(clamp(1.0 - dot(normal, viewDir), 0.0, 1.0), 5.0);

    // 3. SKYBOX ENVIRONMENT REFLECTIONS
    vec3 reflectDir = reflect(-viewDir, normal);
    vec3 reflectionColor = texture(skybox, reflectDir).rgb;

    // 4. DEEP WATER BODY COLOR (Base refraction)
    vec3 deepWaterColor = vec3(0.002, 0.12, 0.20); // Dark marine blue

    // Interpolate refracted color and reflected sky based on the viewing angle (Fresnel)
    vec3 surfaceColor = mix(deepWaterColor, reflectionColor, fresnel);

    // 5. GGX SPECULAR SUN HIGHLIGHTS
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float roughness = 0.12; // Controls the tight spread of the sun glare
    float D = DistributionGGX(normal, halfwayDir, roughness);
    vec3 specular = vec3(1.0, 0.96, 0.90) * D * 0.12;

    vec3 finalColor = surfaceColor + specular;

    // 6. DYNAMIC ENVIRONMENT FOG COLOR
    vec3 fogColor;
    if (u_cameraPos.y < u_heightFogMax) {
        // Submerged: vertical color gradient based on viewing angle
        float depthBlend = clamp((-viewDir.y - (-0.2)) / 1.0, 0.0, 1.0);
        fogColor = mix(u_fogColor, u_heightFogColor, depthBlend).xyz;

        float cameraDepthFactor = clamp(exp(u_cameraPos.y * 0.08), 0.0, 1.0);
        fogColor *= cameraDepthFactor;
    } else {
        // Above water: fade to matching horizon sky color
        fogColor = vec3(0.4, 0.6, 0.9);
    }

    // Apply exponential distance fog
    float dist = length(u_cameraPos.xyz - worldPos);
    float fogDensity = 0.035;
    float fogFactor = clamp(exp(-dist * fogDensity), 0.0, 1.0);

    vec3 foggedColor = mix(fogColor, finalColor, fogFactor);

    // 7. HORIZON AND GRAZING TRANSPARENCY FADING
    float finalAlpha = mix(1.0, 0.6, fogFactor);
    if (u_cameraPos.y < u_heightFogMax) {
        // Fade the water surface to 0% opacity as the view gets horizontal
        float viewAngleFactor = clamp(dot(normal, viewDir), 0.0, 1.0);
        finalAlpha *= pow(viewAngleFactor, 2.0);
    }

    FragColor = vec4(foggedColor, finalAlpha);
    bloomColor = vec4(foggedColor * u_bloomStrength, finalAlpha);
}