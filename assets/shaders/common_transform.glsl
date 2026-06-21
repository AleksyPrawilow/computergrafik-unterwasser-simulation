layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

// Instancing inputs mapped from instanceVBO
layout(location = 5) in mat4 instanceMatrix;

out vec3 vecNormal;
out vec3 worldPos;
out vec3 worldNormal;
out vec2 texCoord;
out mat3 TBN;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform bool u_useInstancing = false;

void transformVertex(
    vec3 localPos,
    vec3 localNormal,
    vec2 localTexCoord,
    vec3 localTangent,
    vec3 localBitangent
) {
    // Select the model matrix dynamically
    mat4 finalModel = u_useInstancing ? instanceMatrix : modelMatrix;

    vec4 world = finalModel * vec4(localPos, 1.0);
    worldPos = world.xyz;

    vecNormal = vec3(finalModel * vec4(localNormal, 0.0));

    vec3 T = normalize((finalModel * vec4(localTangent, 0.0)).xyz);
    vec3 B = normalize((finalModel * vec4(localBitangent, 0.0)).xyz);
    vec3 N = normalize((finalModel * vec4(localNormal, 0.0)).xyz);

    TBN = mat3(T, B, N);
    worldNormal = N;
    texCoord = localTexCoord;

    // FIX: Conditionally select the coordinate space to multiply by
    if (u_useInstancing) {
        gl_Position = transformation * vec4(worldPos, 1.0); // transformation is VP
    } else {
        gl_Position = transformation * vec4(localPos, 1.0); // transformation is MVP
    }
}