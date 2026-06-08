//
// Created by Alexey Pravilov on 08/06/2026.
//

#include "ubootHeadlight.h"

#include "gtx/wrap.inl"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void UbootHeadlight::init() {
    material.albedo = Kern::LoadTexture("assets/textures/sub_albedo.png");
    material.roughness = Kern::LoadTexture("assets/textures/sub_metallic.png");
    material.metallic = Kern::LoadTexture("assets/textures/sub_metallic.png");
    material.normal = Kern::LoadTexture("assets/textures/sub_normal.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
        );
    loadModel("assets/models/uboot_headlight.obj");

    timer = new Timer();
    addChild(timer);

    timer->startTimer(1.0f, [this]() {
        this->onTimerEnd();
    });

    spotlight = LightManager::getInstance().createSpotLight(
        glm::vec3(1.0f, 0.95f, 0.8f), 100.0f, 12.5f, 17.5f
    );
}

void UbootHeadlight::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        transform.pitch(1.0f * deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        transform.pitch(-1.0f * deltaTime);
    }

    spotlight->position = getGlobalTransform().position;
    spotlight->direction = getGlobalTransform().forward();
}

void UbootHeadlight::onTimerEnd() {
    spotlight->intensity = spotlight->intensity > 0 ? 0.0f : 100.0f;
    timer->startTimer(1.0f, [this]() {
        this->onTimerEnd();
    });
}
