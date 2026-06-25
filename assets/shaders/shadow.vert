#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 5) in mat4 instanceMatrix;

uniform mat4 u_lightMVP;
uniform bool u_useInstancing = false;
uniform float u_swayTime = 0.0;

out vec2 texCoord;

void main() {
    vec3 pos = vertexPosition;

    mat4 model = u_useInstancing ? instanceMatrix : mat4(1.0);

    if (u_useInstancing) {
        float heightFactor = clamp(pos.y / 3.0, 0.0, 1.0);
        heightFactor *= heightFactor;
        vec3 worldOrigin = vec3(model[3]);
        float offset = worldOrigin.x * 0.37 + worldOrigin.z * 0.53;
        pos.x += sin(u_swayTime * 1.5 + offset) * 0.95 * heightFactor
               + sin(u_swayTime * 0.4 + offset * 2.3) * 0.4 * heightFactor;
        pos.z += cos(u_swayTime * 1.1 + offset * 1.7) * 0.72 * heightFactor
               + cos(u_swayTime * 0.3 + offset * 3.1) * 0.32 * heightFactor;
    }

    texCoord = vertexTexCoord;

    if (u_useInstancing) {
        gl_Position = u_lightMVP * model * vec4(pos, 1.0);
    } else {
        gl_Position = u_lightMVP * vec4(pos, 1.0);
    }
}
