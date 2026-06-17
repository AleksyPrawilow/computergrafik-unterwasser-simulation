#version 410 core

#include "common_pbr.glsl"

void main() {
    outColor = calculatePBR();
}