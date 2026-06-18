#version 410 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 bloomColor;
uniform float u_bloomStrength = 0.0;

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

uniform vec3 cameraPos;
uniform float time;

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

uniform mat4 inverseModelMatrix;
uniform mat4 modelMatrix;

#include "pbr_lighting.glsl"

float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

float sdCappedCylinder(vec3 p, float h, float r) {
    vec2 d = abs(vec2(length(p.xz), p.y)) - vec2(r, h);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdDome(vec3 p, float radius) {
    float sphere = length(p) - radius;
    float plane = p.y;
    return max(sphere, -plane);
}

float mapScene(vec3 p) {
    vec3 localP = p * 1.5;

    localP.y += sin(time * 2.0 + localP.x * 3.0) * 0.05;

    float dome = sdDome(localP - vec3(0.0, 0.3, 0.0), 0.8);

    vec3 tentacleP = localP;
    tentacleP.x += sin(tentacleP.y * 5.0 + time * 3.0) * 0.1;
    tentacleP.z += cos(tentacleP.y * 4.0 + time * 2.5) * 0.1;

    float centerTentacle = sdCappedCylinder(tentacleP - vec3(0.0, -0.4, 0.0), 0.7, 0.15);

    return smin(dome, centerTentacle, 0.3) / 1.5;
}

//float mapScene(vec3 p) {
//    // Scale the local coordinate space
//    vec3 localP = p * 1.5;
//
//    // A. HOLLOW TRANSLUCENT BELL (The Head)
//    // We subtract a smaller dome from an outer dome to make it hollow!
//    float outerDome = sdDome(localP - vec3(0.0, 0.2, 0.0), 0.7);
//    float innerDome = sdDome(localP - vec3(0.0, 0.17, 0.0), 0.64);
//    float dome = max(outerDome, -innerDome); // Hollow subtraction
//
//    // B. ANATOMICAL ORAL ARMS (Thicker, highly wavy center tentacles)
//    vec3 coreP = localP;
//    coreP.x += sin(coreP.y * 5.0 - time * 3.5) * 0.08;
//    coreP.z += cos(coreP.y * 4.0 - time * 3.0) * 0.08;
//    float centerCore = sdCappedCylinder(coreP - vec3(0.0, -0.25, 0.0), 0.45, 0.08);
//
//    // Combine head and oral arms smoothly
//    float body = smin(dome, centerCore, 0.12);
//
//    // C. 8 TAPERED, OUTER TENTACLES (Polar Repetition!)
//    vec3 tentacleP = localP;
//
//    // Rotate the 3D space around the Y-axis into 8 repeating circular slices
//    float angle = 2.0 * PI / 8.0; // 8 tentacles
//    float a = atan(tentacleP.z, tentacleP.x) + angle / 2.0;
//    float r = length(tentacleP.xz);
//    float c = floor(a / angle);
//    a = mod(a, angle) - angle / 2.0;
//
//    // Re-assemble the rotated XZ coordinates
//    tentacleP.xz = vec2(cos(a), sin(a)) * r;
//
//    // Offset the tentacles outwards so they hang from the rim of the dome (radius ~0.45)
//    tentacleP.x -= 0.46;
//
//    // Tip-Waving: Tentacles wave mostly at the bottom (tips) and remain stable at the top
//    float waveFactor = clamp(0.2 - tentacleP.y, 0.0, 1.5); // Increases as Y goes down (negative)
//    tentacleP.x += sin(tentacleP.y * 7.0 - time * 4.5) * 0.06 * waveFactor;
//    tentacleP.z += cos(tentacleP.y * 6.0 - time * 4.0) * 0.06 * waveFactor;
//
//    // Tapering: Make the tentacles get progressively thinner toward the bottom
//    float dynamicRadius = 0.025 * clamp((tentacleP.y + 0.6) / 1.0, 0.15, 1.0);
//
//    float outerTentacles = sdCappedCylinder(tentacleP - vec3(0.0, -0.4, 0.0), 0.6, dynamicRadius);
//
//    // D. SMOOTHLY BLEND EVERYTHING TOGETHER
//    // 1.5 divisor compensates for the coordinate scaling done at the top
//    return smin(body, outerTentacles, 0.12) / 1.5;
//}

vec3 getNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    float d = mapScene(p);
    vec3 n = vec3(
    d - mapScene(p - e.xyy),
    d - mapScene(p - e.yxy),
    d - mapScene(p - e.yyx)
    );
    return normalize(n);
}

void main() {
    vec3 localCamPos = (inverseModelMatrix * vec4(cameraPos, 1.0)).xyz;
    vec3 localWorldPos = (inverseModelMatrix * vec4(worldPos, 1.0)).xyz;

    bool cameraInside = true;
    cameraInside = abs(localCamPos.x) < 0.5 && abs(localCamPos.y) < 0.5 && abs(localCamPos.z) < 0.5;

    vec3 rayOrigin = cameraInside ? localCamPos : localWorldPos;
    vec3 rayDir = normalize(localWorldPos - localCamPos);

    float distanceTraveled = 0.0;
    const int MAX_STEPS = 32;
    const float MAX_DIST = 2.0;
    const float SURF_DIST = 0.002;

    vec3 hitPoint;
    bool hit = false;

    for(int i = 0; i < MAX_STEPS; i++) {
        hitPoint = rayOrigin + rayDir * distanceTraveled;
        float d = mapScene(hitPoint);
        distanceTraveled += d;

        if(d < SURF_DIST) {
            hit = true;
            break;
        }
        if(distanceTraveled > MAX_DIST) break;
    }

    if(hit) {
        vec3 N = getNormal(hitPoint);
        vec3 V = normalize(localCamPos - hitPoint);

        vec3 albedo = vec3(0.1, 0.6, 0.9);
        float roughness = 0.15;
        float metallic = 0.0;
        vec3 F0 = mix(vec3(0.04), albedo, metallic);

        vec3 hitPointWorld = (modelMatrix * vec4(hitPoint, 1.0)).xyz;

        float ambientScale = u_ambientEnergy;
        if (u_heightFogEnabled) {
            ambientScale = mix(u_ambientEnergy * 0.1, u_ambientEnergy, smoothstep(u_heightFogMin, u_heightFogMax, cameraPos.y));
        }

        float depthFactor = 1.0;
        if (u_heightFogEnabled && hitPointWorld.y < u_heightFogMax) {
            depthFactor = clamp(exp((hitPointWorld.y - u_heightFogMax) * u_depthDimmingCoefficient), 0.01, 1.0);
        }

        vec3 ambient = u_ambientColor * ambientScale * albedo * depthFactor;
        vec3 Lo = vec3(0.0);

        vec3 sunDir = normalize(u_sunDirection);
        vec3 sunRadiance = u_sunColor * u_sunEnergy * depthFactor;
        Lo += CalculateCookTorrance(N, V, sunDir, sunRadiance, albedo, roughness, metallic, F0, hitPoint);

        for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
            vec3 L = normalize(pointLights[i].position - hitPointWorld);
            float distance = length(pointLights[i].position - hitPointWorld);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;
            Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, hitPoint);
        }

        for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
            vec3 L = normalize(spotLights[i].position - hitPointWorld);
            float theta = dot(-L, normalize(spotLights[i].direction));
            float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
            float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
            float distance = length(spotLights[i].position - hitPointWorld);
            float attenuation = 1.0 / (distance * distance + 0.5);
            float absorption = exp(-distance * 0.15);
            vec3 radiance = spotLights[i].color * spotLights[i].intensity * attenuation * intensity * absorption;
            Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, hitPoint);
        }

        vec3 color = ambient + Lo;

        float fresnel = pow(1.0 - max(dot(N, V), 0.0), 4.0);
        vec3 bioluminescence = vec3(0.6, 0.1, 1.0) * fresnel * 3.0;
        color += bioluminescence;

        color = color / (color + vec3(1.0));
        color = pow(color, vec3(1.0 / 2.2));

        if (u_fogEnabled && cameraPos.y < u_heightFogMax) {
            float dist = length(cameraPos - hitPointWorld);

            float currentDrift = sin(hitPointWorld.x * 0.08 + time * 0.3)
            * cos(hitPointWorld.z * 0.08 - time * 0.2)
            * sin(hitPointWorld.y * 0.04);

            float baseFogDensity = u_baseFogDensity;
            float dynamicDensity = baseFogDensity + (currentDrift * 0.008);
            float fogFactor = clamp(exp(-dist * dynamicDensity), 0.0, 1.0);

            vec3 worldRayDir = normalize(hitPointWorld - cameraPos);

            float depthBlend = clamp((worldRayDir.y - (-0.2)) / 1.0, 0.0, 1.0);
            vec3 baseWaterColor = mix(u_fogColor, u_heightFogColor, depthBlend);

            float viewSunAngle = max(dot(worldRayDir, sunDir), 0.0);
            vec3 sunHazeColor = u_sunColor * pow(viewSunAngle, 6.0) * 0.3;

            vec3 finalFogColor = baseWaterColor + sunHazeColor;

            for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
                if (spotLights[i].intensity > 0.0) {
                    vec3 L = normalize(spotLights[i].position - hitPointWorld);
                    float theta = dot(-L, normalize(spotLights[i].direction));
                    float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
                    float spotIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

                    float distanceToLight = length(spotLights[i].position - hitPointWorld);
                    float attenuation = 1.0 / (distanceToLight * distanceToLight + 0.5);
                    float absorption = exp(-distanceToLight * 0.15);

                    vec3 fogGlow = spotLights[i].color * spotLights[i].intensity * attenuation * spotIntensity * 0.15 * absorption;
                    finalFogColor += fogGlow;
                }
            }

            float cameraDepthFactor = 1.0;
            if (u_depthDimmingEnabled && u_heightFogEnabled && cameraPos.y < u_heightFogMax) {
                cameraDepthFactor = clamp(exp((cameraPos.y - u_heightFogMax) * u_depthDimmingCoefficient), 0.0, 1.0);
            }
            finalFogColor *= cameraDepthFactor;

            finalFogColor = finalFogColor / (finalFogColor + vec3(1.0));
            finalFogColor = pow(finalFogColor, vec3(1.0 / 2.2));

            color = mix(finalFogColor, color, fogFactor);
        }

        FragColor = vec4(color, 0.75);
        bloomColor = vec4(color * u_bloomStrength, 0.75);
    } else {
        discard;
    }
}