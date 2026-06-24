#version 410 core
#include "common_transform.glsl"

uniform float time;

// --- 1. CONFIGURABLE SWIMMING PARAMETERS (Body) ---
uniform float u_swimSpeed = 4.5;
uniform float u_swimFrequency = 1.8;
uniform float u_swimAmplitude = 0.22;

// --- 2. CONFIGURABLE NECK/STEERING PARAMETERS (Head) ---
uniform float u_neckPitch = 0.0;    // Up/Down rotation in radians
uniform float u_neckYaw = 0.0;      // Left/Right rotation in radians
uniform float u_neckPivotZ = 0.0;   // Local Z-coordinate where the neck begins bending
uniform float u_neckLength = 2.5;   // Total length of the neck/head section

// Helper to generate rotation matrix around local X-axis (Pitch)
mat4 rotationX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, c,   s,   0.0,
    0.0, -s,  c,   0.0,
    0.0, 0.0, 0.0, 1.0
    );
}

// Helper to generate rotation matrix around local Y-axis (Yaw)
mat4 rotationY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
    c,   0.0, -s,  0.0,
    0.0, 1.0, 0.0, 0.0,
    s,   0.0, c,   0.0,
    0.0, 0.0, 0.0, 1.0
    );
}

void main() {
    vec3 pos = vertexPosition;
    vec3 normal = vertexNormal;

    // --- 1. CALCULATE CASCADING CROSS-FADE FACTORS ---
    // neckFactor goes from 0.0 (body/shoulder) to 1.0 (head)
    float neckFactor = clamp((vertexPosition.z - u_neckPivotZ) / u_neckLength, 0.0, 1.0);
    float bodyFactor = 1.0 - neckFactor; // Decays to 0.0 on the head

    // --- 2. APPLY BODY SWIMMING WAVES ---
    // Swimming only affects the body section; it completely fades out towards the head
    float wavePhase = vertexPosition.z * u_swimFrequency - time * u_swimSpeed;
    float waveDisplacement = sin(wavePhase) * u_swimAmplitude * bodyFactor;

    // Apply lateral waving
    pos.x += waveDisplacement;

    // Apply standard wave-normal tilt to body scale reflections
    float waveSlope = cos(wavePhase) * u_swimAmplitude * u_swimFrequency * bodyFactor;
    normal.x += waveSlope;

    // --- 3. APPLY BENT NECK ROTATIONS ---
    // If the vertex lies in the neck/head section, we rotate it around the local pivot line
    if (vertexPosition.z > u_neckPivotZ) {
        // Linearly scale rotation angles by neckFactor
        // This arches the neck smoothly over its length instead of turning like a mechanical hinge
        float activePitch = u_neckPitch * neckFactor;
        float activeYaw = u_neckYaw * neckFactor;

        mat4 rotMatrix = rotationY(activeYaw) * rotationX(activePitch);

        // Translate vertex relative to pivot, rotate, and translate back
        vec4 localOffset = vec4(pos.x, pos.y, pos.z - u_neckPivotZ, 1.0);
        vec4 rotatedOffset = rotMatrix * localOffset;

        pos = vec3(rotatedOffset.x, rotatedOffset.y, rotatedOffset.z + u_neckPivotZ);

        // Rotate normal and tangent vectors to align with the bent neck
        normal = mat3(rotMatrix) * normal;
    }

    // 4. Delegate to your global engine transform helper
    transformVertex(
        pos,
        normalize(normal),
        vertexTexCoord,
        vertexTangent,
        vertexBitangent
    );
}