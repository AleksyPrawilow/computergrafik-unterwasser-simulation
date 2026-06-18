#version 410 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D u_sceneTexture;
uniform sampler2D u_bloomTexture;
uniform float u_bloomIntensity;
uniform bool u_bloomEnabled;

void main() {
    vec3 scene = texture(u_sceneTexture, TexCoord).rgb;

    if (u_bloomEnabled) {
        vec3 bloom = texture(u_bloomTexture, TexCoord).rgb;
        scene += bloom * u_bloomIntensity;
    }

    outColor = vec4(scene, 1.0);
}
