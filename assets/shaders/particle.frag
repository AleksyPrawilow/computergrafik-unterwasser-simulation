#version 410 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 bloomColor;
uniform float u_bloomStrength = 0.0;

in vec2 TexCoord;
uniform int u_particleType = 0; // 0 = Bubbles, 1 = Rain

void main() {
    if (u_particleType == 1) {
        float xDist = abs(TexCoord.x - 0.5) * 2.0;
        if (xDist > 1.0) {
            discard;
        }
        float alpha = (1.0 - xDist) * 0.40;
        vec3 rainColor = vec3(0.65, 0.70, 0.75) * alpha;

        FragColor = vec4(rainColor, alpha);
        bloomColor = vec4(0.0);
    }
    else {
        // --- 2. ORIGINAL PROCEDURAL BUBBLE ---
        vec2 localCoord = TexCoord * 2.0 - 1.0;

        float r2 = dot(localCoord, localCoord);
        if (r2 > 1.0) {
            discard;
        }

        float z = sqrt(1.0 - r2);
        vec3 normal = normalize(vec3(localCoord, z));

        vec3 lightDir = normalize(vec3(0.3, 0.8, 0.6));
        vec3 viewDir = vec3(0.0, 0.0, 1.0);
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float specFactor = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
        vec3 specular = vec3(0.95, 0.98, 1.0) * specFactor * 0.9;
        float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 4.0);
        vec3 waterColor = vec3(0.0, 0.05, 0.15);
        vec3 bubbleRefraction = vec3(0.8, 0.95, 1.0);
        vec3 finalColor = mix(waterColor, bubbleRefraction, fresnel * 0.35 + specFactor * 0.8);
        float alpha = clamp(fresnel * 0.75 + specFactor * 0.9, 0.0, 1.0);

        FragColor = vec4(finalColor, alpha);
        bloomColor = vec4(finalColor * u_bloomStrength, alpha);
    }
}