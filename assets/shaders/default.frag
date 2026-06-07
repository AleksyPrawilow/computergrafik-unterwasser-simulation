#version 410 core

#include "common_pbr.glsl"

void main() {
    vec3 pbrColor = calculatePBR();
    outColor = vec4(pbrColor, 1.0);
}