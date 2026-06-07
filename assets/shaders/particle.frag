#version 410 core
out vec4 FragColor;

in vec2 TexCoord;

void main() {
    // 1. Convert TexCoords to local range [-1.0, 1.0]
    vec2 localCoord = TexCoord * 2.0 - 1.0;

    // Calculate distance squared from the center
    float r2 = dot(localCoord, localCoord);
    if (r2 > 1.0) {
        discard; // Discard pixels outside the unit circle
    }

    // 2. RECONSTRUCT 3D NORMAL OF THE SPHERE
    float z = sqrt(1.0 - r2);
    vec3 normal = normalize(vec3(localCoord, z)); // Reconstructed 3D Normal pointing towards camera

    // 3. Dynamic Specular Highlights (directional light in camera space)
    vec3 lightDir = normalize(vec3(0.3, 0.8, 0.6)); // Light shining from top-right-front
    vec3 viewDir = vec3(0.0, 0.0, 1.0);            // Camera is looking straight down Z
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float specFactor = pow(max(dot(normal, halfwayDir), 0.0), 128.0); // Shiny specular glint
    vec3 specular = vec3(0.95, 0.98, 1.0) * specFactor * 0.9;

    // 4. Fresnel Reflection (outer edges of the sphere are highly reflective)
    // As we look toward the edges, normal.z approaches 0.0, making Fresnel approach 1.0
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 4.0);

    // 5. Blending & Translucency
    vec3 waterColor = vec3(0.0, 0.05, 0.15); // Blend with deep sea clear color
    vec3 bubbleRefraction = vec3(0.8, 0.95, 1.0); // Translucent blue-white highlight

    // Mix color based on Fresnel reflection and specular highlights
    vec3 finalColor = mix(waterColor, bubbleRefraction, fresnel * 0.35 + specFactor * 0.8);

    // Opaqueness is high at the edges (Fresnel rim) and at the shiny glint
    float alpha = clamp(fresnel * 0.75 + specFactor * 0.9, 0.0, 1.0);

    FragColor = vec4(finalColor, alpha);
}