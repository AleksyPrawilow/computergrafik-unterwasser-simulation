#version 410 core
out vec4 FragColor;

in vec3 worldPos;
in vec3 worldNormal;

uniform vec3 cameraPos; // Passed from your Renderer

void main() {
    // 1. Basic properties
    vec3 normal = normalize(worldNormal);
    vec3 viewDir = normalize(cameraPos - worldPos);

    // Directional light representing the sun/moon shining down
    vec3 lightDir = normalize(vec3(0.3, 1.0, 0.4));

    // 2. Ambient light (base color of the water)
    vec4 waterColor = vec4(0.0, 0.35, 0.5, 0.6); // Cyan-blue with 60% opacity
    vec3 ambient = waterColor.rgb * 0.4;

    // 3. Diffuse light (how light scatters on the waves)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = waterColor.rgb * diff * 0.6;

    // 4. Specular reflections (the shiny "wet" highlights on wave crests)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specFactor = pow(max(dot(normal, halfwayDir), 0.0), 128.0); // 128 is shininess
    vec3 specular = vec3(0.9, 0.95, 1.0) * specFactor * 0.8; // Bright white highlights

    // 5. Combine and output
    vec3 finalColor = ambient + diffuse + specular;

    // Output with transparency
    FragColor = vec4(finalColor, waterColor.a);
}