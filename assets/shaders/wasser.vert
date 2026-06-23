#version 410 core

#include "common_transform.glsl"

layout (std140) uniform GlobalEnvironment {
    vec4 u_sunDirection;
    vec4 u_sunColor;
    vec4 u_ambientColor;
    vec4 u_fogColor;
    vec4 u_heightFogColor;
    vec4 u_causticsColor;
    vec4 u_cameraPos;

    float u_sunEnergy;
    float u_ambientEnergy;
    float u_baseFogDensity;
    float u_heightFogMin;

    float u_heightFogMax;
    float u_causticsScale;
    float u_causticsIntensity;
    float u_depthDimmingCoefficient;

    float u_time;
    float u_bloomThreshold;
    float u_bloomIntensity;

    int u_fogEnabled;
    int u_heightFogEnabled;
    int u_causticsEnabled;
    int u_depthDimmingEnabled;
    int u_bloomEnabled;
};

struct GerstnerWave {
    vec2 direction;
    float amplitude;
    float steepness;
    float wavelength;
    float speed;
};

const int NUM_WAVES = 3;
uniform GerstnerWave waves[NUM_WAVES];

vec3 calculateGerstnerWave(GerstnerWave wave, vec3 pos, inout vec3 tangent, inout vec3 binangent) {
    float k = 2.0 * 3.14159 / wave.wavelength;
    float c = sqrt(9.81 / k) * wave.speed;
    vec2 d = normalize(wave.direction);
    float f = k * (dot(d, pos.xz) - c * u_time);
    float a = wave.amplitude;
    float q = wave.steepness / (a * k * NUM_WAVES);

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

    return vec3(
    q * a * d.x * cos(f),
    a * sin(f),
    q * a * d.y * cos(f)
    );
}

void main() {
    vec3 displacedPos = vertexPosition;
    vec3 tangent = vec3(1.0, 0.0, 0.0);
    vec3 binangent = vec3(0.0, 0.0, 1.0);
    vec3 displacement = vec3(0.0);

    for (int i = 0; i < NUM_WAVES; ++i) {
        displacement += calculateGerstnerWave(waves[i], vertexPosition, tangent, binangent);
    }

    displacedPos += displacement;

    vec3 localNormal = normalize(cross(binangent, tangent));

    transformVertex(displacedPos, localNormal, vertexTexCoord, tangent, binangent);
}