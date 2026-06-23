#version 410 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D uiTexture;
uniform vec4 textColor;
uniform bool isText;

// Helper to sample the font shape safely across both color and alpha channels
float sampleFont(vec2 uv) {
    vec4 tex = texture(uiTexture, uv);
    return tex.r * tex.a;
}

void main() {
    if (isText) {
        vec2 texelSize = vec2(1.0) / textureSize(uiTexture, 0);

        float outlineThickness = 1.0;
        vec2 offset = texelSize * outlineThickness;

        float center = sampleFont(TexCoords);

        float n_up    = sampleFont(TexCoords + vec2(0.0, offset.y));
        float n_down  = sampleFont(TexCoords - vec2(0.0, offset.y));
        float n_left  = sampleFont(TexCoords - vec2(offset.x, 0.0));
        float n_right = sampleFont(TexCoords + vec2(offset.x, 0.0));

        float outlineFactor = max(max(n_up, n_down), max(n_left, n_right));
        outlineFactor = max(outlineFactor, center);

        vec4 outlineColor = vec4(0.0, 0.0, 0.0, textColor.a * outlineFactor);

        FragColor = mix(outlineColor, textColor, center);
    } else {
        FragColor = texture(uiTexture, TexCoords);
    }
}