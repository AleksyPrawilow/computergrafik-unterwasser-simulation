#version 410 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float time;
uniform vec3 cameraPos;

void main()
{
    vec3 dir = normalize(TexCoords);
    vec3 distortedCoords = dir;

    if (cameraPos.y < 0.0 && dir.y > 0.0)
    {
        float waveSpeed = time * 1.5;
        float waveStrength = 0.04;
        float waveFrequency = 12.0;

        float offsetX = sin(dir.x * waveFrequency + waveSpeed) * waveStrength;
        float offsetZ = cos(dir.z * waveFrequency + waveSpeed) * waveStrength;
        float horizonFade = smoothstep(0.0, 0.7, dir.y);

        distortedCoords.x += offsetX * horizonFade;
        distortedCoords.z += offsetZ * horizonFade;
    }

    vec4 baseColor = texture(skybox, normalize(distortedCoords));

    if (cameraPos.y < 0.0) {
        vec4 waterColor = vec4(0.0, 0.05, 0.15, 1.0);

        float fogFactor = clamp(smoothstep(-0.2, 0.8, dir.y), 0.0, 1.0);
        vec4 blendedColor = mix(waterColor, baseColor, fogFactor * 0.35);

        vec3 sunDirection = normalize(vec3(0.1, 1.0, 0.15));
        float viewSunAngle = max(dot(dir, sunDirection), 0.0);

        vec3 sunHaze = vec3(0.4, 0.75, 0.9) * pow(viewSunAngle, 6.0) * 0.3;

        FragColor = blendedColor + vec4(sunHaze, 0.0);
    }
    else {
        FragColor = baseColor;
    }
}