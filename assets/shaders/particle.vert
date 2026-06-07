#version 410 core
layout (location = 0) in vec3 vertexPosition;   // Local quad corner (-0.5 to 0.5)
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 instancePosition; // Instanced position (VBO)
layout (location = 3) in float instanceScale;   // Instanced scale (VBO)

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoord = vertexTexCoord;

    // Extract camera Right and Up vectors from the view matrix to billboard the quad
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp    = vec3(view[0][1], view[1][1], view[2][1]);

    // Calculate world space position of the billboard corner
    vec3 worldPos = instancePosition
    + (cameraRight * vertexPosition.x * instanceScale)
    + (cameraUp * vertexPosition.y * instanceScale);

    gl_Position = projection * view * vec4(worldPos, 1.0);
}