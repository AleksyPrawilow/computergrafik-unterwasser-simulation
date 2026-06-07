//
// Created by Alexey Pravilov on 04/06/2026.
//

#include "earth.h"

#include "moon.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Earth::init() {
    material.albedo = Kern::LoadTexture("assets/textures/RockTexture001_ao.png");
    material.normal = Kern::LoadTexture("assets/textures/RockTexture001_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/RockTexture001_metallic.png");
    transform.position = glm::vec3(40.0f, -50.0f, 30.0f);
    transform.scale = glm::vec3(40.0f);
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    loadModel("assets/models/Rock001.obj");

    auto * moon = new Moon();
    addChild(moon);

}

void Earth::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        queueDestroy();
    }

    const auto time = static_cast<float>(glfwGetTime());
    transform.rotation = glm::angleAxis(time * 0.4f, glm::vec3(0, 1, 0));
}