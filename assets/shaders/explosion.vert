#version 410 core
#include "common_transform.glsl"

uniform float time;
uniform float u_progress;

out float vNoise;

// Standard 3D noise helpers
float hash(vec3 p) {
    p = fract(p * 0.3183099 + vec3(0.1, 0.1, 0.1));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float noise(vec3 x) {
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);

    return mix(mix(mix(hash(i + vec3(0,0,0)), hash(i + vec3(1,0,0)), f.x),
                   mix(hash(i + vec3(0,1,0)), hash(i + vec3(1,1,0)), f.x), f.y),
               mix(mix(hash(i + vec3(0,0,1)), hash(i + vec3(1,0,1)), f.x),
                   mix(hash(i + vec3(0,1,1)), hash(i + vec3(1,1,1)), f.x), f.y), f.z);
}

void main() {
    // Generate noise based on vertex Position and time
    vec3 noiseCoords = vertexPosition * 3.5 + vec3(0.0, 0.0, time * 2.5);
    float n = noise(noiseCoords);
    vNoise = n;

    // Displace the local position along the normal vector
    float displacement = n * 0.5 * sin(u_progress * 3.14159);
    vec3 displacedLocalPos = vertexPosition + vertexNormal * displacement;

    // Delegate the math to your global engine transform helper
    transformVertex(
        displacedLocalPos,
        vertexNormal,
        vertexTexCoord,
        vertexTangent,
        vertexBitangent
    );
}