#version 410 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 u_lightMVP;

void main() {
    gl_Position = u_lightMVP * vec4(vertexPosition, 1.0);
}
