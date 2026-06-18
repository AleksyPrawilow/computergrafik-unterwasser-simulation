//
// Created by Alexey Pravilov on 04/06/2026.
//

#include "earth.h"

#include "moon.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "../werkzeuge/visual/tween.h"
#include "werkzeuge/input.h"

void Earth::init() {
    material.albedo = Kern::LoadTexture("assets/textures/RockTexture001_ao.png");
    material.normal = Kern::LoadTexture("assets/textures/RockTexture001_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/RockTexture001_metallic.png");
    boundingRadius = 4.0f;
    transform.scale = glm::vec3(40.0f);
    transform.position = glm::vec3(0.0f, -40.0f, 0.0f);
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );

    loadModel("assets/models/Rock001.obj");

    auto * moon = new Moon();
    addChild(moon);
    createTween()
        ->tweenProperty(&transform.position.y, -42.0f, 1.5f, EaseType::EASE_IN_OUT_SINE)
        ->tweenProperty(&transform.position.y, -40.0f, 1.5f, EaseType::EASE_IN_OUT_SINE)
        ->setLoops(-1);
}

void Earth::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (Input::isKeyJustPressed(GLFW_KEY_V)) {
        queueDestroy();
    }
}