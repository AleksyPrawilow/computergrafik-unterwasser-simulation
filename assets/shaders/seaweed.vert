#version 410 core

#include "common_transform.glsl"

uniform float u_swayTime = 0.0;

void main() {
    vec3 pos = vertexPosition;

    float heightFactor = clamp(pos.y / 3.0, 0.0, 1.0);
    heightFactor *= heightFactor;

    mat4 model = u_useInstancing ? instanceMatrix : modelMatrix;
    vec3 worldOrigin = vec3(model[3]);
    float offset = worldOrigin.x * 0.37 + worldOrigin.z * 0.53;

    float swayX = sin(u_swayTime * 1.5 + offset) * 0.95 * heightFactor
                + sin(u_swayTime * 0.4 + offset * 2.3) * 0.4 * heightFactor;
    float swayZ = cos(u_swayTime * 1.1 + offset * 1.7) * 0.72 * heightFactor
                + cos(u_swayTime * 0.3 + offset * 3.1) * 0.32 * heightFactor;

    pos.x += swayX;
    pos.z += swayZ;

    transformVertex(pos, vertexNormal, vertexTexCoord, vertexTangent, vertexBitangent);
}
