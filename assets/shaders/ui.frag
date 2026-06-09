#version 410 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uiTexture;
uniform bool isText;
uniform vec4 textColor;

void main() {
    vec4 texColor = texture(uiTexture, TexCoord);

    if (isText) {
        FragColor = vec4(textColor.rgb, texColor.r * textColor.a);
    } else {
        FragColor = texColor;
    }
}