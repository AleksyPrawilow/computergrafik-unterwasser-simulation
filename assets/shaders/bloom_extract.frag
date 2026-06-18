#version 410 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D u_bloomTexture;
uniform float u_threshold;

void main() {
    vec3 color = texture(u_bloomTexture, TexCoord).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > u_threshold) {
        outColor = vec4(color, 1.0);
    } else {
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
