#include "asteroid.h"

#include "../werkzeuge/textur.h"
#include "werkzeuge/random.h"
#include "werkzeuge/shaderManager.h"

Asteroid::Asteroid(float orbitRadius, float orbitGeschwindigkeit, float drehGeschwindigkeit)
    : orbitRadius(orbitRadius)
    , orbitGeschwindigkeit(orbitGeschwindigkeit)
    , drehGeschwindigkeit(drehGeschwindigkeit)
    , drehAchse(glm::normalize(glm::vec3(Random::randf() - 0.5f, Random::randf() - 0.5f, Random::randf() - 0.5f)))
    , zeitVersatz(Random::range(0.0f, 6.28f)) {
    name = "asteroid";
}

void Asteroid::init() {
    loadModel("assets/models/Rock001.obj");
    material.albedo = Kern::LoadTexture("assets/textures/RockTexture001_ao.png");
    material.normal = Kern::LoadTexture("assets/textures/RockTexture001_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/RockTexture001_metallic.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    boundingRadius = 4.0f * transform.scale.x;
    addToGroup("asteroiden");
}

void Asteroid::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    const auto zeit = static_cast<float>(glfwGetTime());
    transform.rotation = glm::angleAxis(zeit * drehGeschwindigkeit, drehAchse);

    if (orbitRadius > 0.0f) {
        transform.position.x = glm::cos(zeit * orbitGeschwindigkeit + zeitVersatz) * orbitRadius;
        transform.position.z = glm::sin(zeit * orbitGeschwindigkeit + zeitVersatz) * orbitRadius;
    }
}
