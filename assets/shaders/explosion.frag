#version 410 core
#include "common_pbr.glsl"

in float vNoise;
uniform float u_progress;

void main() {
    // Compute heat values (fire starts hot and cools down to dark smoke)
    float heat = (1.0 - u_progress) * 1.3;
    heat *= (vNoise * 0.75 + 0.25);

    // Color gradient definitions
    vec3 colorWhite  = vec3(1.6, 1.6, 1.4); // Over-saturated white-hot core
    vec3 colorYellow = vec3(1.0, 0.85, 0.15);
    vec3 colorOrange = vec3(0.9, 0.35, 0.0);
    vec3 colorRed    = vec3(0.7, 0.05, 0.0);
    vec3 colorSmoke  = vec3(0.12, 0.10, 0.10);

    vec3 fireColor = vec3(0.0);

    if (heat > 0.85) {
        fireColor = mix(colorYellow, colorWhite, (heat - 0.85) / 0.45);
    } else if (heat > 0.55) {
        fireColor = mix(colorOrange, colorYellow, (heat - 0.55) / 0.30);
    } else if (heat > 0.35) {
        fireColor = mix(colorRed, colorOrange, (heat - 0.35) / 0.20);
    } else {
        fireColor = mix(colorSmoke, colorRed, heat / 0.35);
    }

    // Alpha dissipation: parts with low heat (ash) fade and dissolve first
    float alpha = smoothstep(1.0, 0.3, u_progress);
    if (heat < 0.2) {
        alpha *= (heat / 0.2);
    }

    // Fresnel Edge Softening (Uses normal vector N from TBN matrix out of transform)
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 normal = normalize(TBN[2]);
    float edgeFactor = dot(normal, viewDir);
    edgeFactor = clamp(1.0 - abs(edgeFactor), 0.0, 1.0);
    alpha *= (1.0 - pow(edgeFactor, 3.0));

    vec3 finalColor = fireColor;

    // --- INTEGRATED UNDERWATER EFFECT 1: COLOR ABSORPTION ---
    if (u_heightFogEnabled && cameraPos.y < u_heightFogMax) {
        vec3 absorptionFilter = vec3(0.35, 0.75, 1.0);
        finalColor *= absorptionFilter;
    }

    // --- INTEGRATED UNDERWATER EFFECT 2: DISTANCE WATER FOG & DIMMING ---
    if (u_fogEnabled && cameraPos.y < u_heightFogMax) {
        float dist = length(cameraPos - worldPos);

        // Match the organic waving fog drift defined in common_pbr.glsl
        float currentDrift = sin(worldPos.x * 0.08 + time * 0.3)
        * cos(worldPos.z * 0.08 - time * 0.2)
        * sin(worldPos.y * 0.04);

        float dynamicDensity = u_baseFogDensity + (currentDrift * 0.008);
        float fogFactor = clamp(exp(-dist * dynamicDensity), 0.0, 1.0);

        // Calculate depth-blended background water color
        float depthBlend = clamp((-viewDir.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseWaterColor = mix(u_fogColor, u_heightFogColor, depthBlend);

        // Dim the background fog color by the camera's depth
        if (u_depthDimmingEnabled) {
            float cameraDepthFactor = clamp(exp(cameraPos.y * u_depthDimmingCoefficient), 0.0, 1.0);
            baseWaterColor *= cameraDepthFactor;
        }

        // Apply standard HDR tonemapping and gamma correction to the background fog color
        baseWaterColor = baseWaterColor / (baseWaterColor + vec3(1.0));
        baseWaterColor = pow(baseWaterColor, vec3(1.0 / 2.2));

        // Blend the fire color into the background fog
        finalColor = mix(baseWaterColor, finalColor, fogFactor);
    }

    outColor = vec4(finalColor, alpha);
}