//
// Created by Alexey Pravilov on 25/06/2026.
//

#include "seaweed.h"

#include <GLFW/glfw3.h>
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Seaweed::init() {
    loadModel("assets/models/seaweed.obj");
    material.shader = ShaderManager::getInstance().loadShader(
        "seaweed", "assets/shaders/seaweed.vert", "assets/shaders/default.frag");
    material.albedo = Kern::LoadTexture("assets/textures/seaweed.png");
    material.opacity = Kern::LoadTexture("assets/textures/seaweed.png");
    material.doubleSided = true;
}

void Seaweed::prepareUniforms() const {
    Kern::setUniform(material.shader, "alphaCutoff", 0.1f);
    Kern::setUniform(material.shader, "u_swayTime", static_cast<float>(glfwGetTime()));
}
