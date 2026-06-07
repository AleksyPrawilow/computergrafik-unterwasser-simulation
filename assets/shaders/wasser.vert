#version 410 core

// 1. Inherit all standard layout inputs, outputs (including TBN/texCoord), and matrices!
#include "common_transform.glsl"

uniform float time;

// Define a structure for customizable Gerstner Waves
struct GerstnerWave {
    vec2 direction;
    float amplitude;
    float steepness;
    float wavelength;
    float speed;
};

const int NUM_WAVES = 3;
uniform GerstnerWave waves[NUM_WAVES];

// Helper function to calculate a single Gerstner wave contribution
vec3 calculateGerstnerWave(GerstnerWave wave, vec3 pos, inout vec3 tangent, inout vec3 binangent) {
    float k = 2.0 * 3.14159 / wave.wavelength;
    float c = sqrt(9.81 / k) * wave.speed; // Speed of wave based on gravity
    vec2 d = normalize(wave.direction);
    float f = k * (dot(d, pos.xz) - c * time);
    float a = wave.amplitude;
    float q = wave.steepness / (a * k * NUM_WAVES); // Normalize steepness

    // Calculate tangent and binangent derivatives for analytical normals
    tangent += vec3(
    -q * d.x * d.x * k * a * sin(f),
    d.x * k * a * cos(f),
    -q * d.x * d.y * k * a * sin(f)
    );
    binangent += vec3(
    -q * d.x * d.y * k * a * sin(f),
    d.y * k * a * cos(f),
    -q * d.y * d.y * k * a * sin(f)
    );

    // Return the actual 3D displacement
    return vec3(
    q * a * d.x * cos(f),
    a * sin(f),
    q * a * d.y * cos(f)
    );
}

void main() {
    // 2. Initialize local calculation variables using the inherited 'vertexPosition' input
    vec3 displacedPos = vertexPosition;
    vec3 tangent = vec3(1.0, 0.0, 0.0);
    vec3 binangent = vec3(0.0, 0.0, 1.0);
    vec3 displacement = vec3(0.0);

    // Accumulate waves
    for (int i = 0; i < NUM_WAVES; ++i) {
        displacement += calculateGerstnerWave(waves[i], vertexPosition, tangent, binangent);
    }

    // Apply displacement to get final local vertex position
    displacedPos += displacement;

    // Calculate the local normal using the updated tangent and binangent
    vec3 localNormal = normalize(cross(binangent, tangent));

    // 3. Pass the wave-displaced position, the normal, and the wave tangents
    // to the shared function. It will automatically calculate TBN, worldPos,
    // texCoord, and gl_Position for the fragment shader!
    transformVertex(displacedPos, localNormal, vertexTexCoord, tangent, binangent);
}