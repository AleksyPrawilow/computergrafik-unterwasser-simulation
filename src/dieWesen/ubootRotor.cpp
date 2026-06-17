//
// Created by Alexey Pravilov on 07/06/2026.
//

#include "ubootRotor.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void UbootRotor::init() {
    material.albedo = Kern::LoadTexture("assets/textures/sub_albedo.png");
    material.roughness = Kern::LoadTexture("assets/textures/sub_metallic.png");
    material.metallic = Kern::LoadTexture("assets/textures/sub_metallic.png");
    material.normal = Kern::LoadTexture("assets/textures/sub_normal.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
        );
    loadModel("assets/models/uboot_rotor.obj");
}

void UbootRotor::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    transform.roll(rotorSpeed * deltaTime);
}
