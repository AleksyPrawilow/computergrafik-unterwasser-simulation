#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation; // MVP matrix
uniform mat4 modelMatrix;    // Model matrix
uniform float time;          // Pass glfwGetTime() from C++

out vec3 vecNormal;
out vec3 worldPos;
out vec3 worldNormal;
out vec2 texCoord;
out mat3 TBN; // Tangent-Bitangent-Normal matrix to fragment shader

// Helper function to deform the local position
vec3 deformTentacle(vec3 pos, float t)
{
    vec3 modified = pos;

    // Change this value to the coordinate where the tentacles start on your model.
    // If your squid is vertical, this will be a Y coordinate.
    float threshold = 5;

    if (pos.y < threshold)
    {
        float distanceDown = abs(pos.y - threshold);

        // Rhythmic wave on X and Z axes
        float waveX = sin(t * 3.0 + pos.y * 0.5) * 0.4;
        float waveZ = cos(t * 3.0 + pos.y * 0.5) * 0.4;

        modified.x += waveX * distanceDown;
        modified.z += waveZ * distanceDown;
    }
    return modified;
}

void main()
{
    // 1. Calculate the deformed local position
    vec3 modifiedPosition = deformTentacle(vertexPosition, time);

    // 2. Transform the MODIFIED position to world space
    vec4 world = modelMatrix * vec4(modifiedPosition, 1.0);
    worldPos = world.xyz;

    vecNormal = vec3(modelMatrix * vec4(vertexNormal, 0.0));

    vec3 T = normalize((modelMatrix * vec4(vertexTangent, 0.0)).xyz);
    vec3 B = normalize((modelMatrix * vec4(vertexBitangent, 0.0)).xyz);
    vec3 N = normalize((modelMatrix * vec4(vertexNormal, 0.0)).xyz);

    TBN = mat3(T, B, N);

    worldNormal = N;
    texCoord = vertexTexCoord;

    // 3. Transform the MODIFIED position to clip space
    gl_Position = transformation * vec4(modifiedPosition, 1.0);
}