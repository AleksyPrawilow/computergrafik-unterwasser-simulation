//
// Created by Alexey Pravilov on 04/06/2026.
//

#include "wasser.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Wasser::init() {
    loadModel("assets/models/wasser.obj");
    material.shader = ShaderManager::getInstance().loadShader(
        "wasser",
        "assets/shaders/wasser.vert",
        "assets/shaders/wasser.frag"
    );
    material.normal = Kern::LoadTexture("assets/textures/water_normal.png");
    material.isTransparent = true;
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
}

void Wasser::prepareUniforms() const {
    const GLuint s = material.shader;

    Kern::setUniform(s, "waves[0].direction", glm::vec2(1.0f, 0.1f));
    Kern::setUniform(s, "waves[0].amplitude", 0.25f);
    Kern::setUniform(s, "waves[0].steepness", 0.5f);
    Kern::setUniform(s, "waves[0].wavelength", 12.0f);
    Kern::setUniform(s, "waves[0].speed", 1.5f);

    Kern::setUniform(s, "waves[1].direction", glm::vec2(0.2f, 1.0f));
    Kern::setUniform(s, "waves[1].amplitude", 0.15f);
    Kern::setUniform(s, "waves[1].steepness", 0.5f);
    Kern::setUniform(s, "waves[1].wavelength", 6.0f);
    Kern::setUniform(s, "waves[1].speed", 1.0f);

    Kern::setUniform(s, "waves[2].direction", glm::vec2(-0.5f, 0.5f));
    Kern::setUniform(s, "waves[2].amplitude", 0.08f);
    Kern::setUniform(s, "waves[2].steepness", 0.5f);
    Kern::setUniform(s, "waves[2].wavelength", 3.0f);
    Kern::setUniform(s, "waves[2].speed", 0.8f);
}


