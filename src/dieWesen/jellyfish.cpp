#include "jellyfish.h"
#include "../werkzeuge/shaderManager.h"
#include "../werkzeuge/textur.h"

void Jellyfish::init() {
    material.albedo = Kern::LoadTexture("assets/textures/jellyfish.png");

    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(1.0f);

    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/shader_5_1_tex.vert",
        "assets/shaders/shader_5_1_tex.frag"
    );

    loadModel("assets/models/jellyfish.obj");
}

void Jellyfish::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    transform.position.y = sin(static_cast<float>(glfwGetTime())) * 0.5f;
    transform.rotation = glm::angleAxis(static_cast<float>(glfwGetTime()), glm::vec3(0, 1, 0));
}