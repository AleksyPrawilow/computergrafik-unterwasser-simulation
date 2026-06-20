#version 410 core
layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 ortho;
uniform vec2 uvOffset;
uniform bool isText;

void main() {
    TexCoords = isText ? (vertexTexCoord / 16.0) + uvOffset : vertexTexCoord;
    gl_Position = ortho * model * vec4(vertexPosition, 0.0, 1.0);
}