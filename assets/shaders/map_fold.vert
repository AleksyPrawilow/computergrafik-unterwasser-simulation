#version 410 core
#include "common_transform.glsl"

uniform float time;
uniform float u_progress; // 0.0 = fully rolled scroll, 1.0 = flat open map

void main() {
    vec3 pos = vertexPosition;
    vec3 normal = vertexNormal;

    // 1. Calculate the target cylinder radius based on a width of 1.0
    float PI = 3.14159265359;
    float R = 1.0 / (2.0 * PI); // ~0.15915

    // 2. Map the local horizontal X coordinate to a radial angle
    float theta = vertexPosition.x / R;

    // 3. Compute the perfectly rolled cylinder coordinates in the XY plane
    float rolledX = R * sin(theta);
    float rolledY = R * (1.0 - cos(theta)); // Bends upward along the Y-axis

    // 4. Interpolate coordinates (X and Y are rolled, Z stays completely untouched)
    pos.x = mix(rolledX, vertexPosition.x, u_progress);
    pos.y = mix(rolledY, vertexPosition.y, u_progress);
    pos.z = vertexPosition.z; // FIX: Keep Z-axis (length) 100% static to prevent twisting

    // 5. Rotate the normals in the XY plane to match the cylinder's curvature
    vec3 rolledNormal = vec3(sin(theta), cos(theta), 0.0);
    normal = normalize(mix(rolledNormal, vertexNormal, u_progress));

    // 6. Pass the final clean coordinates
    transformVertex(
        pos,
        normal,
        vertexTexCoord,
        vertexTangent,
        vertexBitangent
    );
}