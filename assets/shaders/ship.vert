#version 410 core

uniform float time;
uniform float deformationThreshold;

#include "common_transform.glsl"

vec3 deformTentacle(vec3 pos, float t)
{
    vec3 modified = pos;

    if (pos.y < deformationThreshold)
    {
        float distanceDown = abs(pos.y - deformationThreshold);

        float waveX = sin(t * 3.0 + pos.y * 0.5) * 0.4;
        float waveZ = cos(t * 3.0 + pos.y * 0.5) * 0.4;

        modified.x += waveX * distanceDown;
        modified.z += waveZ * distanceDown;
    }
    return modified;
}

void main()
{
    vec3 modifiedPosition = deformTentacle(vertexPosition, time);
    transformVertex(modifiedPosition, vertexNormal, vertexTexCoord, vertexTangent, vertexBitangent);
}