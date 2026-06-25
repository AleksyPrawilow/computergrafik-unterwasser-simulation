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

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

uniform samplerCube skybox;  // Binds your global cubemap texture!
uniform sampler2D normalMap; // Binds your water_normal.png

void main() {
    vec3 viewDir = normalize(u_cameraPos.xyz - worldPos);
    vec3 I = -viewDir;

    // 1. Sliding water droplets normal map (Only scroll if caustics/water are enabled!)
    vec2 uvScroll = texCoord * 3.0 + vec2(0.0, -u_time * 0.05);

    vec3 tangentNormal = texture(normalMap, uvScroll).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);

    // Highly flattened so it only gently warps specular highlights and edges
    tangentNormal.xy *= 0.04;
    vec3 N = normalize(TBN * tangentNormal);

    // 2. Calculate Reflection
    vec3 R = reflect(I, N);
    vec3 reflectionColor = texture(skybox, R).rgb;

    // 3. Calculate Refraction (Water to Glass)
    // Refractive index ratio: water (1.333) to glass (1.52) = 0.87
    // Switch to 1.0 / 1.52 (0.65) in C++ or a uniform if in the vacuum of space!
    float eta = (u_cameraPos.y < u_heightFogMax) ? 0.87 : 0.65;
    vec3 T = refract(I, N, eta);
    vec3 refractionColor = texture(skybox, T).rgb;

    // --- 4. APPLY DYNAMIC ENVIRONMENTAL FOG TO SAMPLED CUBEMAP PIXELS ---
    if (u_fogEnabled == 1 && u_cameraPos.y < u_heightFogMax) {
        // Apply vertical gradient fog to the refracted view
        float refractFog = clamp((T.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseRefractColor = mix(u_fogColor.xyz, u_heightFogColor.xyz, refractFog);
        refractionColor = mix(baseRefractColor, refractionColor, refractFog * 0.35);

        // Apply vertical gradient fog to the reflected view
        float reflectFog = clamp((R.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseReflectColor = mix(u_fogColor.xyz, u_heightFogColor.xyz, reflectFog);
        reflectionColor = mix(baseReflectColor, reflectionColor, reflectFog * 0.35);

        // Add the sun god-ray/haze glow through the glass when looking up
        vec3 sunDirection = normalize(u_sunDirection.xyz);
        float viewSunAngle = max(dot(T, sunDirection), 0.0);
        vec3 sunHaze = u_sunColor.xyz * pow(viewSunAngle, 6.0) * 0.3;
        refractionColor += sunHaze;

        // Apply camera depth-dimming to both reflection and refraction
        if (u_depthDimmingEnabled == 1) {
            float cameraDepthFactor = clamp(exp((u_cameraPos.y - u_heightFogMax) * u_depthDimmingCoefficient), 0.0, 1.0);
            refractionColor *= cameraDepthFactor;
            reflectionColor *= cameraDepthFactor;
        }
    }

    // 5. Fresnel Effect
    float F0 = 0.08;
    float Fresnel = F0 + (1.0 - F0) * pow(1.0 - max(dot(N, viewDir), 0.0), 5.0);

    // Apply Look-Down Glare Fix: fade out reflections when looking down
    if (viewDir.y > 0.0) {
        float lookDownFactor = clamp(1.0 - viewDir.y * 1.5, 0.0, 1.0);
        Fresnel *= lookDownFactor;
    }

    // Blend reflection and refraction
    vec3 finalColor = mix(refractionColor, reflectionColor, Fresnel);

    vec3 glassTint = vec3(0.03, 0.06, 0.08);
    finalColor += glassTint;

    // --- 6. APPlY TONEMAPPING & GAMMA CORRECTION TO THE GLASS COLOR ---
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    // 7. Distance Fog (blends the physical glass geometry with the environment)
    vec3 foggedColor = finalColor;
    float dist = length(u_cameraPos.xyz - worldPos);
    float fogFactor = clamp(exp(-dist * u_baseFogDensity), 0.0, 1.0);

    if (u_fogEnabled == 1 && u_cameraPos.y < u_heightFogMax) {
        // Calculate the moving, dynamic fog color at the glass's position
        float currentDrift = sin(worldPos.x * 0.08 + u_time * 0.3)
        * cos(worldPos.z * 0.08 - u_time * 0.2)
        * sin(worldPos.y * 0.04);

        float dynamicDensity = u_baseFogDensity + (currentDrift * 0.008);
        float localFogFactor = clamp(exp(-dist * dynamicDensity), 0.0, 1.0);

        float depthBlend = clamp((-viewDir.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseWaterColor = mix(u_fogColor.xyz, u_heightFogColor.xyz, depthBlend);

        vec3 sunDirection = normalize(u_sunDirection.xyz);
        float viewSunAngle = max(dot(-viewDir, sunDirection), 0.0);
        vec3 sunHazeColor = u_sunColor.xyz * pow(viewSunAngle, 6.0) * 0.3;

        vec3 finalFogColor = baseWaterColor + sunHazeColor;

        // Apply camera depth-dimming to the final fog
        if (u_depthDimmingEnabled == 1) {
            float cameraDepthFactor = clamp(exp((u_cameraPos.y - u_heightFogMax) * u_depthDimmingCoefficient), 0.0, 1.0);
            finalFogColor *= cameraDepthFactor;
        }

        // Tonemap and Gamma Correct the fog color independently (matches PBR objects)
        finalFogColor = finalFogColor / (finalFogColor + vec3(1.0));
        finalFogColor = pow(finalFogColor, vec3(1.0 / 2.2));

        // Blend the glass with the fog
        foggedColor = mix(finalFogColor, finalColor, localFogFactor);
    }

    float alpha = mix(0.85, 0.3, 1.0 - Fresnel);

    if (!gl_FrontFacing) {
        alpha = mix(0.85, 0.35, 1.0 - Fresnel);
    }

    FragColor = vec4(foggedColor, alpha);
    bloomColor = vec4(foggedColor * u_bloomStrength, alpha);
}