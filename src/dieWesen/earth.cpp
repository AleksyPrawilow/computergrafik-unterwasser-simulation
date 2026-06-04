//
// Created by Alexey Pravilov on 04/06/2026.
//

#include "earth.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Earth::init() {
    material.albedo = Kern::LoadTexture("assets/textures/earth.png");
    material.normal = Kern::LoadTexture("assets/textures/earth_normal.png");
    transform.position = glm::vec3(40.0f, 50.0f, 30.0f);
    transform.scale = glm::vec3(28.0f);
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/shader_5_1_tex.vert",
        "assets/shaders/shader_5_1_tex.frag"
    );
    loadModel("assets/models/sphere.obj");

    rock = new Wesen();
    rock->loadModel("assets/models/Rock001.obj");
    rock->material.shader = ShaderManager::getInstance().getShader("default");
    rock->material.albedo = Kern::LoadTexture("assets/textures/RockTexture001_ao.png");
    rock->material.normal = Kern::LoadTexture("assets/textures/RockTexture001_normal.png");
    rock->material.metallic = Kern::LoadTexture("assets/textures/RockTexture001_metallic.png");
    rock->transform.position = glm::vec3(0);
    rock->transform.scale = glm::vec3(2.0f);

    addChild(rock);
}

void Earth::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (rock != nullptr) {
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            rock->queueDestroy();
            rock = nullptr;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        queueDestroy();
    }

    const auto time = static_cast<float>(glfwGetTime());
    transform.rotation = glm::angleAxis(time * 0.4f, glm::vec3(0, 1, 0));

    if (rock != nullptr) {
        rock->transform.rotation = glm::angleAxis(time, glm::vec3(0, 1, 0));
        rock->transform.position.x = cos(-time * 0.5f) * 4;
        rock->transform.position.z = sin(-time * 0.5f) * 4;
    }
}