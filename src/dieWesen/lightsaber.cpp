//
// Created by Alexey Pravilov on 24/06/2026.
//

#include "lightsaber.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

Lightsaber::Lightsaber(const std::string& color) {
    bladeColor = color;
}

void Lightsaber::init() {
    loadModel("assets/models/lightsaber_hilt.obj");
    material.albedo = Kern::LoadTexture("assets/textures/lightsaber_albedo.png");
    material.normal = Kern::LoadTexture("assets/textures/lightsaber_normal.png");
    material.roughness = Kern::LoadTexture("assets/textures/lightsaber_metallicRoughness.png");
    material.albedo = Kern::LoadTexture("assets/textures/lightsaber_metallicRoughness.png");
    material.shader = ShaderManager::getInstance().getShader("default");

    blade = new Wesen();
    blade->loadModel("assets/models/lightsaber_blade.obj");
    blade->material.albedo = Kern::LoadTexture(bladeColor.c_str());
    blade->material.emission = Kern::LoadTexture(bladeColor.c_str());
    blade->material.isTransparent = true;
    blade->material.bloomStrength = 4.0f;
    blade->material.shader = ShaderManager::getInstance().getShader("default");
    addChild(blade);

    euler.x = 45.0f;
}

void Lightsaber::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    // euler.y += 25.0f * deltaTime;
    // transform.rotation = glm::quat(glm::radians(euler));
    transform.pitch(10.0f * deltaTime * dir);
}
