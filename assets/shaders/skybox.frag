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
uniform float u_bloomStrength = 0.0;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    vec3 dir = normalize(TexCoords);
    vec3 distortedCoords = dir;

    // FIX: Calculate an ambient-aware brightness scale
    // This dims the skybox during storms and brightens it during lightning flashes
    float skyBrightness = clamp((u_sunEnergy * 0.35) + (u_ambientEnergy * 1.0), 0.02, 1.15);

    // Apply wave surface distortion if camera is submerged AND we are looking UP
    if (u_heightFogEnabled != 0 && u_cameraPos.y < u_heightFogMax && dir.y > 0.0)
    {
        float waveSpeed = u_time * 1.5;
        float waveStrength = 0.04;
        float waveFrequency = 12.0;

        float offsetX = sin(dir.x * waveFrequency + waveSpeed) * waveStrength;
        float offsetZ = cos(dir.z * waveFrequency + waveSpeed) * waveStrength;
        float horizonFade = smoothstep(0.0, 0.7, dir.y);

        distortedCoords.x += offsetX * horizonFade;
        distortedCoords.z += offsetZ * horizonFade;
    }

    // Sample and apply dynamic environmental brightness to the skybox texture
    vec4 baseColor = texture(skybox, normalize(distortedCoords));
    vec3 litSkyColor = baseColor.rgb * skyBrightness;

    // If submerged, calculate physical water column fog
    if (u_heightFogEnabled != 0 && u_cameraPos.y < u_heightFogMax)
    {
        float waterDistance;
        if (dir.y > 0.0001) {
            waterDistance = (u_heightFogMax - u_cameraPos.y) / dir.y;
        } else {
            waterDistance = 10000.0;
        }

        float skyboxFog = clamp(exp(-waterDistance * u_baseFogDensity), 0.0, 1.0);

        // Height-blended water color
        float depthBlend = clamp((dir.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseWaterColor = mix(u_fogColor.xyz, u_heightFogColor.xyz, depthBlend);
        vec4 waterColor = vec4(baseWaterColor, 1.0);

        // Dynamic Sun Haze (Scaled by the sun's active color/energy)
        vec3 sunDirection = u_sunDirection.xyz;
        float viewSunAngle = max(dot(dir, sunDirection), 0.0);
        vec3 sunHaze = u_sunColor.xyz * u_sunEnergy * pow(viewSunAngle, 6.0) * 0.15;

        // Blend skybox with water color
        vec4 finalColor = mix(waterColor, vec4(litSkyColor, 1.0), skyboxFog);
        finalColor += vec4(sunHaze, 0.0);

        vec3 finalOutputColor = finalColor.rgb;
        if (u_depthDimmingEnabled == 1) {
            float depthFactor = clamp(exp(u_cameraPos.y * u_depthDimmingCoefficient), 0.0, 1.0);
            finalOutputColor *= depthFactor;
        }

        // Apply Tonemapping & Gamma Correction
        finalOutputColor = finalOutputColor / (finalOutputColor + vec3(1.0));
        finalOutputColor = pow(finalOutputColor, vec3(1.0 / 2.2));

        FragColor = vec4(finalOutputColor, 1.0);
        bloomColor = vec4(finalOutputColor * u_bloomStrength, 1.0);
    }
    else
    {
        // Standard clean sky above water (Dimmed/Brightened dynamically)
        FragColor = vec4(litSkyColor, baseColor.a);
        bloomColor = vec4(litSkyColor * u_bloomStrength, 1.0);
    }
}