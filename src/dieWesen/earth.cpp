//
// Created by Alexey Pravilov on 04/06/2026.
//

#include "earth.h"

#include "moon.h"
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