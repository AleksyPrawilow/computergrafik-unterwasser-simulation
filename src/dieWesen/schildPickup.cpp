#include "schildPickup.h"
#include "raumschiff.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/audioManager.h"

SchildPickup::SchildPickup(const glm::vec3& position)
    : startPosition(position) {}

void SchildPickup::init() {
    loadModel("assets/models/sphere.obj");
    material.albedo = Kern::LoadTexture("assets/textures/emission_cyan.png");
    material.emission = Kern::LoadTexture("assets/textures/emission_cyan.png");
    material.bloomStrength = 0.5f;
    material.isTransparent = true;
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.position = startPosition;
    transform.scale = glm::vec3(2.0f);
    boundingRadius = 3.0f;
    addToGroup("pickups");
    name = "schildPickup";
}

void SchildPickup::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    alter += deltaTime;
    if (alter >= lebensdauer) {
        queueDestroy();
        return;
    }

    transform.position.y = startPosition.y + glm::sin(alter * 3.0f) * 1.5f;
    transform.rotation = glm::angleAxis(alter * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    const auto& spieler = getNodesInGroup("spieler");
    for (auto* s : spieler) {
        float abstand = glm::distance(getGlobalTransform().position, s->getGlobalTransform().position);
        if (abstand < boundingRadius + s->boundingRadius) {
            if (auto* schiff = dynamic_cast<Raumschiff*>(s)) {
                schiff->heilen(25.0f);
            }
            AudioManager::getInstance().play2D("assets/audio/pickup.mp3", false, true);
            queueDestroy();
            return;
        }
    }
}
