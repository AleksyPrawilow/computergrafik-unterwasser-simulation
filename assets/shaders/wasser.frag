#version 410 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 bloomColor;
uniform float u_bloomStrength = 0.0;

in vec3 worldPos;
in vec3 worldNormal;

uniform vec3 cameraPos;
uniform float time;

// Universal Environment Uniforms
uniform vec3 u_fogColor;
uniform vec3 u_heightFogColor;
uniform float u_heightFogMin;
uniform float u_heightFogMax;

void main() {
    // 1. Basic properties
    vec3 normal = normalize(worldNormal);
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 lightDir = normalize(vec3(0.3, 1.0, 0.4)); // Directional sun/moon

    // 2. Base Water Color
    vec3 surfaceColor = vec3(0.0, 0.35, 0.5);

    // 3. Ambient & Diffuse lighting on the waves
    vec3 ambient = surfaceColor * 0.4;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = surfaceColor * diff * 0.6;

    // 4. Specular reflections (the wet highlights)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specFactor = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
    vec3 specular = vec3(0.9, 0.95, 1.0) * specFactor * 0.8;

    vec3 finalColor = ambient + diffuse + specular;

    // 5. --- DYNAMIC ENVIRONMENT FOG COLOR ---
    vec3 fogColor;
    if (cameraPos.y < u_heightFogMax) {
        // CAMERA IS UNDERWATER: Calculate vertical gradient
        float depthBlend = clamp((-viewDir.y - (-0.2)) / 1.0, 0.0, 1.0);
        fogColor = mix(u_fogColor, u_heightFogColor, depthBlend);

        float cameraDepthFactor = clamp(exp(cameraPos.y * 0.08), 0.0, 1.0);
        fogColor *= cameraDepthFactor;
    } else {
        // CAMERA IS ABOVE WATER: Fade to the lighter above-water sky color
        fogColor = vec3(0.4, 0.6, 0.9);
    }

    // 6. Apply exponential distance fog
    float dist = length(cameraPos - worldPos);
    float fogDensity = 0.035;
    float fogFactor = clamp(exp(-dist * fogDensity), 0.0, 1.0);

    // Blend the final water color with the dynamic environment fog
    vec3 foggedColor = mix(fogColor, finalColor, fogFactor);

    // --- 7. FIXED: FADE WATER SURFACE TO 0% OPACITY AT HORIZON ---
    float finalAlpha = mix(1.0, 0.6, fogFactor);
    if (cameraPos.y < u_heightFogMax) {
        // If submerged, fade the water surface to 0% opacity as the viewing angle
        // becomes horizontal (perpendicular to normal)
        float viewAngleFactor = clamp(dot(normal, viewDir), 0.0, 1.0);

        // Power of 2.0 makes the fade transition incredibly soft and seamless
        finalAlpha *= pow(viewAngleFactor, 2.0);
    }

    FragColor = vec4(foggedColor, finalAlpha);
    bloomColor = vec4(foggedColor * u_bloomStrength, finalAlpha);
}