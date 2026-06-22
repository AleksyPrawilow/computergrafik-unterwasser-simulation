#version 410 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 bloomColor;

// FIX: Renamed from TexCoords to match common_transform.glsl's output exactly
in vec2 texCoord;

uniform float time;
uniform float u_progress; // 0.0 = top of sky, 1.0 = struck the ground
uniform vec4 u_color = vec4(0.85, 0.93, 1.0, 1.0); // Electric blue-white

// Simple 2D procedural noise helper
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

// 4-Octave Fractal Brownian Motion (FBM) to create jagged branches
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 4; i++) {
        value += amplitude * noise(p);
        p *= 2.1;
        amplitude *= 0.5;
    }
    return value;
}

void main() {
    // FIX: Read from texCoord
    vec2 uv = texCoord;

    // 1. Animate the bolt striking downwards based on progress
    float strikeFront = 1.0 - u_progress;
    if (uv.y < strikeFront) {
        discard;
    }

    // 2. Generate highly jagged offset path using FBM noise
    float jag = fbm(vec2(uv.y * 8.0, time * 1.5)) * 0.35;
    jag      += fbm(vec2(uv.y * 24.0, time * 3.5)) * 0.08; // High-frequency micro-jaggedness

    // Add a low-frequency curving factor to make the bolt snake organically
    float curve = sin(uv.y * 3.0 + time * 0.5) * 0.12;

    // Center of the bolt is 0.5 (middle of the quad)
    float centerOffset = 0.5 + jag + curve;
    float dist = abs(uv.x - centerOffset);

    // 3. Render a white-hot core and a soft surrounding electric glow
    float core = exp(-dist * 130.0);       // Tight, blinding white center
    float glow = exp(-dist * 15.0) * 0.45; // Soft atmospheric halo

    vec3 finalGlow = u_color.rgb * (core + glow);

    // Animate the bolt's opacity so it flickers and fades out at the end of progress
    float alpha = clamp((core + glow) * (1.0 - u_progress), 0.0, 1.0);

    if (alpha < 0.05) {
        discard;
    }

    FragColor = vec4(finalGlow, alpha);
    bloomColor = vec4(finalGlow * 1.8, alpha);
}