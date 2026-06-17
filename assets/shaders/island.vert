#version 410 core
#include "common_transform.glsl"

const float PI = 3.14159265359;

// --- DOME ISLAND GENERATOR ---
// Generates a smooth, circular island of radius 12.0m
float getIslandHeight(float x, float z) {
    float d = length(vec2(x, z));
    float radius = 24.0; // 12-meter radius island

    if (d < radius) {
        float factor = d / radius;
        // Smooth cosine curve: Peak is at 1.5m above water (since we subtract 1.0, and water is at 0.0)
        return cos(factor * (PI / 2.0)) * 2.5 - 1.0;
    }
    return -30.0; // Submerged sandy seabed
}

void main() {
    vec3 displacedPos = vertexPosition;

    // Deform the flat grid vertically to create the island dome!
    displacedPos.y = getIslandHeight(vertexPosition.x, vertexPosition.z);

    // Calculate the slope derivatives to reconstruct perfect lighting normals
    float h_left  = getIslandHeight(vertexPosition.x - 0.1, vertexPosition.z);
    float h_right = getIslandHeight(vertexPosition.x + 0.1, vertexPosition.z);
    float h_down  = getIslandHeight(vertexPosition.x, vertexPosition.z - 0.1);
    float h_up    = getIslandHeight(vertexPosition.x, vertexPosition.z + 0.1);

    // Cross product normal approximation
    vec3 normal = normalize(vec3(h_left - h_right, 0.2, h_down - h_up));

    transformVertex(displacedPos, normal, vertexTexCoord, vertexTangent, vertexBitangent);
}