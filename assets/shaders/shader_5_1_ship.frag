#version 410 core

uniform sampler2D colorTexture;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap; // New
uniform sampler2D metallicMap;  // New

uniform vec3 lightPos;
uniform vec3 cameraPos;

float AMBIENT = 0.2; // Adjusted slightly for PBR look

in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

out vec4 outColor;

void main()
{
    // 1. Get and transform normal from the normal map
    vec3 tangentNormal = texture(normalMap, texCoord).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);
    vec3 normal = normalize(TBN * tangentNormal);

    // 2. Sample GLB-packaged PBR textures
    // GLB standard: Roughness is in Green (g), Metalness is in Blue (b)
    float roughness = texture(roughnessMap, texCoord).g;
    float metallic  = texture(metallicMap, texCoord).b;

    // 3. Directions for light and view
    vec3 lightDir = normalize(lightPos - worldPos);
    vec3 viewDir  = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    // 4. Sample base color texture (Albedo)
    vec3 albedo = texture(colorTexture, texCoord).rgb;

    // 5. Calculate dynamic shininess based on roughness
    // Low roughness (0.0) -> high shininess (128.0)
    // High roughness (1.0) -> low shininess (4.0)
    float dynamicShininess = mix(128.0, 4.0, roughness);

    // 6. Basic Lighting calculations
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), dynamicShininess);

    // 7. Apply Metalness rules
    // Rule A: Metals reflect specular highlights in their own color; non-metals reflect white
    vec3 specularColor = mix(vec3(1.0), albedo, metallic);

    // Rule B: Metals absorb diffuse light, so scale down diffuse based on metallic value
    vec3 diffuse = (diff * albedo) * (1.0 - metallic);

    // 8. Combine lighting components
    vec3 ambient = AMBIENT * albedo * (1.0 - metallic * 0.5); // Metals have darker ambient
    vec3 specular = spec * specularColor * (1.0 - roughness * 0.5); // Rougher surfaces reflect less light

    // 9. Final Color Output
    vec3 color = ambient + diffuse + specular;
    outColor = vec4(color, 1.0);
}