#version 410 core
layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;

out vec2 TexCoord;

uniform mat4 ortho;
uniform mat4 model;

uniform bool isText;
uniform vec2 uvOffset;

void main() {
    if (isText) {
        TexCoord = vertexTexCoord / 16.0 + uvOffset;
    } else {
        TexCoord = vertexTexCoord;
    }
    gl_Position = ortho * model * vec4(vertexPosition, 0.0, 1.0);
}