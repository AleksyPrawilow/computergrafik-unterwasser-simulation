#version 410 core

#include "common_pbr.glsl"

void main() {
    outColor = calculatePBR();
    bloomColor = vec4(outColor.rgb * u_bloomStrength, outColor.a);
}