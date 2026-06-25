#version 410 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 instancePosition;
layout (location = 3) in vec2 instanceScale;
layout (location = 4) in float instanceLife;

out vec2 TexCoord;
out float vLife;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoord = vertexTexCoord;
    vLife = instanceLife;

    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 worldPos = instancePosition
    + (cameraRight * vertexPosition.x * instanceScale.x)
    + (cameraUp * vertexPosition.y * instanceScale.y);

    gl_Position = projection * view * vec4(worldPos, 1.0);
}