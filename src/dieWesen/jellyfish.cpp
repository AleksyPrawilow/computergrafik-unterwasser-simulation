#include "jellyfish.h"
#include "../werkzeuge/shaderManager.h"

void Jellyfish::init() {
    material.isTransparent = true;
    transform.position = glm::vec3(1.0f, 0.0f, 1.0f);
    transform.scale = glm::vec3(2.0f);
    material.shader = ShaderManager::getInstance().loadShader(
        "jellyfish",
        "assets/shaders/default.vert",
        "assets/shaders/raymarch.frag"
    );
    material.isTransparent = true;
    material.bloomStrength = 0.6f;
    loadModel("assets/models/cube.obj");
}

void Jellyfish::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    transform.position.y = sin(static_cast<float>(glfwGetTime())) * 0.5f - 50.0f;
}