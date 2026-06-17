#version 410 core

#include "common_transform.glsl"

void main() {
    transformVertex(vertexPosition, vertexNormal, vertexTexCoord, vertexTangent, vertexBitangent);
}