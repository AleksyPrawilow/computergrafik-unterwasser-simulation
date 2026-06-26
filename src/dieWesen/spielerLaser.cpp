#include "spielerLaser.h"
#include "alien.h"
#include "explosion.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"

void SpielerLaser::init() {
    loadModel("assets/models/cube.obj");
    transform.scale = glm::vec3(0.05f, 0.05f, 2.0f);
    material.albedo = Kern::LoadTexture("assets/textures/torpedo_blau.png");
    material.emission = Kern::LoadTexture("assets/textures/torpedo_blau.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.bloomStrength = 0.8f;
    boundingRadius = 0.3f;
    addToGroup("spielerlaser");
    name = "spielerLaser";
    visible = false;

    selbstzerstoerungsTimer = new Timer();
    addChild(selbstzerstoerungsTimer);
}

void SpielerLaser::abfeuern(const glm::vec3& position, const glm::quat& rotation) {
    transform.position = position;
    transform.rotation = rotation;
    istAbgefeuert = true;
    visible = true;
    updateGlobalTransforms();
    selbstzerstoerungsTimer->startTimer(lebensdauer, [this]() { queueDestroy(); });
}

void SpielerLaser::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!istAbgefeuert) return;

    transform.position += transform.forward() * geschwindigkeit * deltaTime;

    const auto& aliens = getNodesInGroup("aliens");
    for (auto* ziel : aliens) {
        float abstand = glm::distance(getGlobalTransform().position, ziel->getGlobalTransform().position);
        if (abstand < boundingRadius + ziel->boundingRadius) {
            if (auto* alien = dynamic_cast<Alien*>(ziel)) {
                alien->schadenNehmen(1.0f);
            }
            if (parent != nullptr) {
                parent->addChild(new Explosion(ziel->getGlobalTransform().position, 2.0f));
            }
            queueDestroy();
            return;
        }
    }
}
