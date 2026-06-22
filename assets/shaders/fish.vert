#version 410 core
#include "common_transform.glsl"


uniform float time;
uniform float u_swimSpeed = 8.0;
uniform float u_swimFrequency = 4.5;
uniform float u_swimAmplitude = 0.15;

void main() {
    vec3 displacedPos = vertexPosition;
    vec3 displacedNormal = vertexNormal;

    float rampFactor = clamp(0.4 - vertexPosition.z, 0.0, 1.2);
    float wavePhase = vertexPosition.z * u_swimFrequency - time * u_swimSpeed;
    float waveDisplacement = sin(wavePhase) * u_swimAmplitude * rampFactor;

    displacedPos.x += waveDisplacement;
    float waveSlope = cos(wavePhase) * u_swimAmplitude * u_swimFrequency * rampFactor;
    displacedNormal.x += waveSlope;
    displacedNormal = normalize(displacedNormal);

    transformVertex(
        displacedPos,
        displacedNormal,
        vertexTexCoord,
        vertexTangent,
        vertexBitangent
    );
}