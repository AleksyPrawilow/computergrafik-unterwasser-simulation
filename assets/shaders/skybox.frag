#version 410 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float time;
uniform vec3 cameraPos; // We will pass this from C++ now

void main()
{
    vec3 dir = normalize(TexCoords);
    vec3 distortedCoords = dir;

    // Apply wave surface distortion if camera is submerged AND we are looking UP
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

    // If submerged, fade the skybox into the deep water fog color
    if (cameraPos.y < 0.0)
    {
        vec4 waterColor = vec4(0.0, 0.05, 0.15, 1.0); // Matches glClearColor

        // Fade the fog thinner looking straight up (Snell's window)
        // and thicker looking horizontally toward the deep water horizon
        float fogFactor = clamp(smoothstep(-0.2, 0.8, dir.y), 0.0, 1.0);

        FragColor = mix(waterColor, baseColor, fogFactor * 0.35);
    }
    else
    {
        // Standard clean sky above water
        FragColor = baseColor;
    }
}