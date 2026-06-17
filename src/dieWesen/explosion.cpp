#include "explosion.h"

#include "../werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"

Explosion::Explosion(const glm::vec3& position, float groesse)
    : startPosition(position), maxGroesse(groesse) {
    name = "explosion";
}

void Explosion::init() {
    transform.position = startPosition;
    transform.scale = glm::vec3(0.1f);

    loadModel("assets/models/sphere.obj");
    material.albedo = Kern::LoadTexture("assets/textures/laser_rot.png");
    material.emission = Kern::LoadTexture("assets/textures/laser_rot.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default", "assets/shaders/default.vert", "assets/shaders/default.frag"
    );

    cachedGlobalModelMatrix = transform.getModelMatrix();
    cachedGlobalTransform = transform;
}

void Explosion::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    alter += deltaTime;
    float t = alter / lebensdauer;

    if (t >= 1.0f) {
        queueDestroy();
        return;
    }

    float skala = maxGroesse * glm::sin(t * 3.14159f);
    transform.scale = glm::vec3(skala);
}
