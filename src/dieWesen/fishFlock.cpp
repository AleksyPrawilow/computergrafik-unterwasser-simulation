#include "fishFlock.h"
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

        fish->transform.scale = glm::vec3(15.0f);

        addChild(fish);
    }
}

void FishFlock::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {

}