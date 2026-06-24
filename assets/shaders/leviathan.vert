#version 410 core
#include "common_transform.glsl"

uniform float time;

uniform float u_swimSpeed = 4.5;
uniform float u_swimFrequency = 1.8;
uniform float u_swimAmplitude = 0.22;

uniform float u_neckPitch = 0.0;
uniform float u_neckYaw = 0.0;
uniform float u_neckPivotZ = 1.5;
uniform float u_neckLength = 6.0;

mat4 rotationX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(1.0, 0.0, 0.0, 0.0, 0.0, c, s, 0.0, 0.0, -s, c, 0.0, 0.0, 0.0, 0.0, 1.0);
}

mat4 rotationY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(c, 0.0, -s, 0.0, 0.0, 1.0, 0.0, 0.0, s, 0.0, c, 0.0, 0.0, 0.0, 0.0, 1.0);
}

void main() {
    vec3 pos = vertexPosition;
    vec3 normal = vertexNormal;

    float neckFactor = clamp((vertexPosition.z - u_neckPivotZ) / u_neckLength, 0.0, 1.0);
    float bodyFactor = 1.0 - neckFactor;

    float wavePhase = vertexPosition.z * u_swimFrequency - time * u_swimSpeed;
    float waveDisplacement = sin(wavePhase) * u_swimAmplitude * bodyFactor;
    pos.x += waveDisplacement;

    float waveSlope = cos(wavePhase) * u_swimAmplitude * u_swimFrequency * bodyFactor;
    normal.x += waveSlope;

    if (vertexPosition.z > u_neckPivotZ) {
        float activePitch = u_neckPitch * neckFactor;
        float activeYaw = u_neckYaw * neckFactor;

        mat4 rotMatrix = rotationY(activeYaw) * rotationX(activePitch);

        vec4 localOffset = vec4(pos.x, pos.y, pos.z - u_neckPivotZ, 1.0);
        vec4 rotatedOffset = rotMatrix * localOffset;

        pos = vec3(rotatedOffset.x, rotatedOffset.y, rotatedOffset.z + u_neckPivotZ);
        normal = mat3(rotMatrix) * normal;
    }

    transformVertex(
        pos,
        normalize(normal),
        vertexTexCoord,
        vertexTangent,
        vertexBitangent
    );
}