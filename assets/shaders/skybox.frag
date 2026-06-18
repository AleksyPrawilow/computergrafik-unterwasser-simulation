#version 410 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 bloomColor;
uniform float u_bloomStrength = 0.0;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float time;
uniform vec3 cameraPos;

// Universal Environment Uniforms
uniform vec3 u_fogColor;
uniform vec3 u_heightFogColor;
uniform float u_heightFogMin;
uniform float u_heightFogMax;
uniform float u_baseFogDensity;
uniform float u_heightFogEnabled = 1.0;
uniform bool u_depthDimmingEnabled = true;
uniform float u_depthDimmingCoefficient = 0.08f;

void main()
{
    vec3 dir = normalize(TexCoords);
    vec3 distortedCoords = dir;

    // Apply wave surface distortion if camera is submerged AND we are looking UP
    if (u_heightFogEnabled > 0.5 && cameraPos.y < u_heightFogMax && dir.y > 0.0)
    {
        float waveSpeed = time * 1.5;
        float waveStrength = 0.04;
        float waveFrequency = 12.0;

        float offsetX = sin(dir.x * waveFrequency + waveSpeed) * waveStrength;
        float offsetZ = cos(dir.z * waveFrequency + waveSpeed) * waveStrength;
        float horizonFade = smoothstep(0.0, 0.7, dir.y);

        distortedCoords.x += offsetX * horizonFade;
        distortedCoords.z += offsetZ * horizonFade;
    }

    vec4 baseColor = texture(skybox, normalize(distortedCoords));

    // If submerged, calculate physical water column fog
    if (u_heightFogEnabled > 0.5 && cameraPos.y < u_heightFogMax)
    {
        // 1. Calculate physical water column fog
        float waterDistance;
        if (dir.y > 0.0001) {
            waterDistance = (u_heightFogMax - cameraPos.y) / dir.y;
        } else {
            waterDistance = 10000.0;
        }

        float skyboxFog = clamp(exp(-waterDistance * u_baseFogDensity), 0.0, 1.0);

        // Height-blended water color
        float depthBlend = clamp((dir.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseWaterColor = mix(u_fogColor, u_heightFogColor, depthBlend);
        vec4 waterColor = vec4(baseWaterColor, 1.0);

        // Dynamic Sun Haze
        vec3 sunDirection = normalize(vec3(0.1, 1.0, 0.15));
        float viewSunAngle = max(dot(dir, sunDirection), 0.0);
        vec3 sunHaze = vec3(0.4, 0.75, 0.9) * pow(viewSunAngle, 6.0) * 0.3;

        // Blend skybox with water color
        vec4 finalColor = mix(waterColor, baseColor, skyboxFog);
        finalColor += vec4(sunHaze, 0.0);

        vec3 finalOutputColor = finalColor.rgb;
        if (u_depthDimmingEnabled) {
            float depthFactor = clamp(exp(cameraPos.y * u_depthDimmingCoefficient), 0.0, 1.0);
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
        // Standard clean sky above water
        FragColor = baseColor;
        bloomColor = vec4(baseColor.rgb * u_bloomStrength, 1.0);
    }
}