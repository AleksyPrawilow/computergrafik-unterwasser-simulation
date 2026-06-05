//
// Created by Alexey Pravilov on 05/06/2026.
//

#include "moon.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Moon::init() {
    material.shader = ShaderManager::getInstance().getShader("default");
    material.albedo = Kern::LoadTexture("assets/textures/RockTexture001_ao.png");
    material.normal = Kern::LoadTexture("assets/textures/RockTexture001_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/RockTexture001_metallic.png");
    loadModel("assets/models/Rock001.obj");

    transform.position = glm::vec3(0);
    transform.scale = glm::vec3(2.0f);
}

void Moon::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        queueDestroy();
    }

    const auto time = static_cast<float>(glfwGetTime());
    transform.rotation = glm::angleAxis(time, glm::vec3(0, 1, 0));
    transform.position.x = cos(-time * 0.5f) * 4;
    transform.position.z = sin(-time * 0.5f) * 4;
}
