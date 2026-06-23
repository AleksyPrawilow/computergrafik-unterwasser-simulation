#version 410 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D uiTexture;
uniform vec4 textColor;
uniform bool isText;
uniform float u_dissolve;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0.0,0.0)), hash(i + vec2(1.0,0.0)), f.x),
               mix(hash(i + vec2(0.0,1.0)), hash(i + vec2(1.0,1.0)), f.x), f.y);
}

float sampleFont(vec2 uv) {
    vec4 tex = texture(uiTexture, uv);
    return tex.r * tex.a;
}

void main() {
    if (isText) {
        // 1. Procedural noise check. If threshold is met, discard the pixel
        float n = noise(TexCoords * 45.0); // Adjust frequency to scale noise pattern size
        if (n < u_dissolve) {
            discard;
        }

        // 2. Outline sample mapping
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
        vec4 textCol = textColor;

        // 3. Burning glow edge effect
        float borderSize = 0.08;
        if (n < u_dissolve + borderSize) {
            float edgeT = (u_dissolve + borderSize - n) / borderSize;
            // Transitions edge from deep volcanic red to glowing gold
            vec3 glow = mix(vec3(1.3, 0.15, 0.0), vec3(2.0, 1.5, 0.3), edgeT);
            textCol.rgb = glow;
            outlineColor.rgb = glow;
        }

        FragColor = mix(outlineColor, textCol, center);
    } else {
        FragColor = texture(uiTexture, TexCoords);
    }
}