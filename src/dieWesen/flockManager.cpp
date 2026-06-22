#include "flockManager.h"
#include "fishBolid.h"
#include "../werkzeuge/TextureManager.h"
#include "../werkzeuge/ShaderManager.h"

void FishFlock::init() {
    for (int i = 0; i < numFishes; i++) {
        FishBoid* fish = new FishBoid();

        fish->material.shader = ShaderManager::getInstance().loadShader(
            "default",
            "assets/shaders/default.vert",
            "assets/shaders/default.frag"
        );

        fish->loadModel("assets/models/fish.obj");

        fish->material.albedo = TextureManager::getInstance().loadTexture("assets/textures/fish.png");

        fish->transform.scale = glm::vec3(5.0f);

        fish->transform.position = glm::vec3(
            (rand() % 200 / 100.0f - 1.0f) * spawnRadius,
            (rand() % 200 / 100.0f - 1.0f) * spawnRadius,
            (rand() % 200 / 100.0f - 1.0f) * spawnRadius
        );

        float randomScale = 3.0f + (rand() % 100 / 25.0f);
        fish->transform.scale = glm::vec3(randomScale);

        float randomSpeed = 6.0f + (rand() % 100 / 25.0f);
        fish->maxSpeed = randomSpeed;

        fish->maxForce = randomSpeed * 0.5f;
        addChild(fish);
    }
}

void FishFlock::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {

}