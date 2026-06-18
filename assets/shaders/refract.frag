#version 410 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 bloomColor;
uniform float u_bloomStrength = 0.0;

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

uniform vec3 cameraPos;
uniform samplerCube skybox;  // Binds your global cubemap texture!
uniform sampler2D normalMap; // Binds your water_normal.png
uniform float time;

// --- UNIVERSAL ENVIRONMENT UNIFORMS ---
uniform vec3 u_sunDirection;
uniform vec3 u_sunColor;
uniform float u_sunEnergy;

uniform vec3 u_ambientColor;
uniform float u_ambientEnergy;

uniform bool u_fogEnabled;
uniform vec3 u_fogColor;
uniform float u_baseFogDensity;

uniform bool u_heightFogEnabled;
uniform vec3 u_heightFogColor;
uniform float u_heightFogMin;
uniform float u_heightFogMax;

uniform bool u_depthDimmingEnabled = true;
uniform float u_depthDimmingCoefficient = 0.08f;

void main() {
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 I = -viewDir;

    // 1. Sliding water droplets normal map (Only scroll if caustics/water are enabled!)
    vec2 uvScroll = texCoord * 3.0 + vec2(0.0, -time * 0.05);

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
    float eta = (cameraPos.y < u_heightFogMax) ? 0.87 : 0.65;
    vec3 T = refract(I, N, eta);
    vec3 refractionColor = texture(skybox, T).rgb;

    // --- 4. APPLY DYNAMIC ENVIRONMENTAL FOG TO SAMPLED CUBEMAP PIXELS ---
    if (u_fogEnabled && cameraPos.y < u_heightFogMax) {
        // Apply vertical gradient fog to the refracted view
        float refractFog = clamp((T.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseRefractColor = mix(u_fogColor, u_heightFogColor, refractFog);
        refractionColor = mix(baseRefractColor, refractionColor, refractFog * 0.35);

        // Apply vertical gradient fog to the reflected view
        float reflectFog = clamp((R.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseReflectColor = mix(u_fogColor, u_heightFogColor, reflectFog);
        reflectionColor = mix(baseReflectColor, reflectionColor, reflectFog * 0.35);

        // Add the sun god-ray/haze glow through the glass when looking up
        vec3 sunDirection = normalize(u_sunDirection);
        float viewSunAngle = max(dot(T, sunDirection), 0.0);
        vec3 sunHaze = u_sunColor * pow(viewSunAngle, 6.0) * 0.3;
        refractionColor += sunHaze;

        // Apply camera depth-dimming to both reflection and refraction
        if (u_depthDimmingEnabled) {
            float cameraDepthFactor = clamp(exp((cameraPos.y - u_heightFogMax) * u_depthDimmingCoefficient), 0.0, 1.0);
            refractionColor *= cameraDepthFactor;
            reflectionColor *= cameraDepthFactor;
        }
    }

    // 5. Fresnel Effect
    float F0 = 0.02;
    float Fresnel = F0 + (1.0 - F0) * pow(1.0 - max(dot(N, viewDir), 0.0), 5.0);

    // Apply Look-Down Glare Fix: fade out reflections when looking down
    if (viewDir.y > 0.0) {
        float lookDownFactor = clamp(1.0 - viewDir.y * 1.5, 0.0, 1.0);
        Fresnel *= lookDownFactor;
    }

    // Blend reflection and refraction
    vec3 finalColor = mix(refractionColor, reflectionColor, Fresnel);

    // Add a very subtle, high-tech cyan glass tint
    vec3 glassTint = vec3(0.01, 0.03, 0.045);
    finalColor += glassTint;

    // --- 6. APPlY TONEMAPPING & GAMMA CORRECTION TO THE GLASS COLOR ---
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    // 7. Distance Fog (blends the physical glass geometry with the environment)
    vec3 foggedColor = finalColor;
    float dist = length(cameraPos - worldPos);
    float fogFactor = clamp(exp(-dist * u_baseFogDensity), 0.0, 1.0);

    if (u_fogEnabled && cameraPos.y < u_heightFogMax) {
        // Calculate the moving, dynamic fog color at the glass's position
        float currentDrift = sin(worldPos.x * 0.08 + time * 0.3)
        * cos(worldPos.z * 0.08 - time * 0.2)
        * sin(worldPos.y * 0.04);

        float dynamicDensity = u_baseFogDensity + (currentDrift * 0.008);
        float localFogFactor = clamp(exp(-dist * dynamicDensity), 0.0, 1.0);

        float depthBlend = clamp((-viewDir.y - (-0.2)) / 1.0, 0.0, 1.0);
        vec3 baseWaterColor = mix(u_fogColor, u_heightFogColor, depthBlend);

        vec3 sunDirection = normalize(u_sunDirection);
        float viewSunAngle = max(dot(-viewDir, sunDirection), 0.0);
        vec3 sunHazeColor = u_sunColor * pow(viewSunAngle, 6.0) * 0.3;

        vec3 finalFogColor = baseWaterColor + sunHazeColor;

        // Apply camera depth-dimming to the final fog
        if (u_depthDimmingEnabled) {
            float cameraDepthFactor = clamp(exp((cameraPos.y - u_heightFogMax) * u_depthDimmingCoefficient), 0.0, 1.0);
            finalFogColor *= cameraDepthFactor;
        }

        // Tonemap and Gamma Correct the fog color independently (matches PBR objects)
        finalFogColor = finalFogColor / (finalFogColor + vec3(1.0));
        finalFogColor = pow(finalFogColor, vec3(1.0 / 2.2));

        // Blend the glass with the fog
        foggedColor = mix(finalFogColor, finalColor, localFogFactor);
    }

    // Transparency: Opaque at grazing angles (edges), transparent in center
    float alpha = mix(0.80, 0.08, 1.0 - Fresnel);

    if (!gl_FrontFacing) {
        // Double-sided support (First-Person cockpit)
        alpha = mix(0.80, 0.12, 1.0 - Fresnel);
    }

    FragColor = vec4(foggedColor, alpha);
    bloomColor = vec4(foggedColor * u_bloomStrength, alpha);
}