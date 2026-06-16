#version 410 core

#include "pbr_lighting.glsl"

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

out vec4 outColor;

uniform vec3 cameraPos;
uniform float time;

uniform vec3 u_sunDirection;
uniform vec3 u_sunColor;
uniform float u_sunEnergy;
uniform vec3 u_ambientColor;
uniform float u_ambientEnergy;

uniform bool u_heightFogEnabled;
uniform float u_heightFogMin;
uniform float u_heightFogMax;

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
    vec3 rayOrigin = cameraPos;
    vec3 rayDir = normalize(worldPos - cameraPos);

    float distanceTraveled = 0.0;
    const int MAX_STEPS = 80;
    const float MAX_DIST = 30.0;
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
        vec3 V = normalize(cameraPos - hitPoint);

        vec3 albedo = vec3(0.1, 0.6, 0.9);
        float roughness = 0.15;
        float metallic = 0.0;
        vec3 F0 = mix(vec3(0.04), albedo, metallic);


    float ambientScale = u_ambientEnergy;
    if (u_heightFogEnabled) {
    ambientScale = mix(u_ambientEnergy * 0.1, u_ambientEnergy, smoothstep(u_heightFogMin, u_heightFogMax, cameraPos.y));
    }

    float depthFactor = 1.0;
    if (u_heightFogEnabled && hitPoint.y < u_heightFogMax) {
    depthFactor = clamp(exp((hitPoint.y - u_heightFogMax) * 0.08), 0.01, 1.0);
    }

    vec3 ambient = u_ambientColor * ambientScale * albedo * depthFactor;
    vec3 Lo = vec3(0.0);

    vec3 sunRadiance = u_sunColor * u_sunEnergy * depthFactor;
    Lo += CalculateCookTorrance(N, V, u_sunDirection, sunRadiance, albedo, roughness, metallic, F0, hitPoint);

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
    vec3 L = normalize(pointLights[i].position - hitPoint);
    float distance = length(pointLights[i].position - hitPoint);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;
    Lo += CalculateCookTorrance(N, V, L, radiance, albedo, roughness, metallic, F0, hitPoint);
    }

    for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
    vec3 L = normalize(spotLights[i].position - hitPoint);
    float theta = dot(-L, normalize(spotLights[i].direction));
    float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
    float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
    float distance = length(spotLights[i].position - hitPoint);
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

    outColor = vec4(color, 0.75);
    } else {
        discard;
    }
}