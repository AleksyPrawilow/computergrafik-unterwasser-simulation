//
// Created by Alexey Pravilov on 23/06/2026.
//

#include "map.h"

#include "werkzeuge/input.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"

void Map::init() {
    name = "MapScroll";
    loadModel("assets/models/map.obj");
    material.albedo = Kern::LoadTexture("assets/textures/map.png");
    material.shader = ShaderManager::getInstance().loadShader(
            "map_fold",
            "assets/shaders/map_fold.vert",
            "assets/shaders/default.frag"
        );

    unwrapProgress = 0.0f;
}

void Map::prepareUniforms() const {
    Kern::setUniform(material.shader, "u_progress", unwrapProgress);
}

void Map::unwrap(float duration) {
    unwrapProgress = 0.0f;

    createTween()
        ->tweenProperty(&unwrapProgress, 1.0f, duration, EaseType::EASE_OUT_SINE);
}

void Map::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    transform.rotation = glm::quat(glm::radians(mapEuler));
}