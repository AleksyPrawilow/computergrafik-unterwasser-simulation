#version 410 core

layout (location = 0) in vec2 aPos;

uniform vec2 u_Offset;

void main() {
    gl_Position = vec4(aPos + u_Offset, 0.0, 1.0);
}