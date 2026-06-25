#version 410 core

in vec2 texCoord;

uniform sampler2D opacityMap;
uniform bool u_hasOpacity = false;

void main() {
    if (u_hasOpacity) {
        float alpha = texture(opacityMap, texCoord).r;
        if (alpha < 0.5) discard;
    }
}
