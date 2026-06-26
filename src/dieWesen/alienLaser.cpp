#include "alienLaser.h"
#include "player.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"

void AlienLaser::init() {
    loadModel("assets/models/cube.obj");
    transform.scale = glm::vec3(0.08f, 0.08f, 1.5f);
    material.albedo = Kern::LoadTexture("assets/textures/emission_gruen.png");
    material.emission = Kern::LoadTexture("assets/textures/emission_gruen.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.bloomStrength = 0.8f;
    boundingRadius = 0.4f;
    addToGroup("feindlaser");
    name = "alienLaser";
    visible = false;

    selbstzerstoerungsTimer = new Timer();
    addChild(selbstzerstoerungsTimer);
}

void AlienLaser::abfeuern(const glm::vec3& position, const glm::quat& rotation) {
    transform.position = position;
    transform.rotation = rotation;
    istAbgefeuert = true;
    visible = true;
    updateGlobalTransforms();
    selbstzerstoerungsTimer->startTimer(lebensdauer, [this]() { queueDestroy(); });
}

void AlienLaser::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!istAbgefeuert) return;

    transform.position += transform.forward() * geschwindigkeit * deltaTime;

    const auto& spieler = getNodesInGroup("spielerInsel");
    for (auto* ziel : spieler) {
        auto* player = dynamic_cast<Player*>(ziel);
        if (player && player->hatSpawnSchutz()) continue;

        float abstand = glm::distance(getGlobalTransform().position, ziel->getGlobalTransform().position);
        if (abstand < boundingRadius + 1.5f) {
            if (player) player->schadenNehmen(schaden);
            queueDestroy();
            return;
        }
    }
}
