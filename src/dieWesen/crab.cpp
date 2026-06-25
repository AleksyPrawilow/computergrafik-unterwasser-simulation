#include "crab.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Crab::init() {
    material.shader = ShaderManager::getInstance().getShader("default");

    if (type == 1) {
        loadModel("assets/models/crab1.obj");
        material.albedo = Kern::LoadTexture("assets/textures/crab1.png");
        material.normal = Kern::LoadTexture("assets/textures/crab1_normal.png");
        material.metallic = Kern::LoadTexture("assets/textures/crab1_metalness.png");
        material.roughness = material.metallic;
    } else {
        loadModel("assets/models/crab2.obj");
        material.albedo = Kern::LoadTexture("assets/textures/crab2.png");
        material.metallic = Kern::LoadTexture("assets/textures/crab2_metalness.png");
        material.roughness = material.metallic;
    }

    transform.scale = glm::vec3(1.0f);
}

void Crab::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
}