#include "jellyfish.h"
#include "../werkzeuge/shaderManager.h"
#include "../werkzeuge/textur.h"

void Jellyfish::init() {
    material.albedo = Kern::LoadTexture("assets/textures/jellyfish_albedo.png");
    material.emission = Kern::LoadTexture("assets/textures/jellyfish_emission_map.png");
    material.opacity = Kern::LoadTexture("assets/textures/jellyfish_opacity.png");
    material.normal = Kern::LoadTexture("assets/textures/jellyfish_normal_map.png");
    material.isTransparent = true;

    transform.position = glm::vec3(1.0f, 0.0f, 1.0f);
    transform.scale = glm::vec3(0.25f);

    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );

    loadModel("assets/models/jellyfish_outer.obj");

    auto * jellyfishInner = new Wesen();

    jellyfishInner->loadModel("assets/models/cube.obj");

    jellyfishInner->transform.scale = glm::vec3(4.0f);

    jellyfishInner->material.isTransparent = true;

    jellyfishInner->material.shader = ShaderManager::getInstance().loadShader(
        "raymarch",
        "assets/shaders/default.vert",
        "assets/shaders/raymarch.frag"
    );

    addChild(jellyfishInner);
}

void Jellyfish::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    transform.position.y = sin(static_cast<float>(glfwGetTime())) * 0.5f - 50.0f;
    transform.rotation = glm::angleAxis(static_cast<float>(glfwGetTime()), glm::vec3(0, 1, 0));
}