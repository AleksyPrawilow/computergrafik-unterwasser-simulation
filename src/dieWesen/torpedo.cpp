#include "torpedo.h"
#include "explosion.h"
#include "feindschiff.h"

#include "../werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"

void Torpedo::init() {
    loadModel("assets/models/cube.obj");
    transform.scale = glm::vec3(0.12f, 0.12f, 2.5f);
    material.albedo = Kern::LoadTexture("assets/textures/torpedo_blau.png");
    material.emission = Kern::LoadTexture("assets/textures/torpedo_blau.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    material.bloomStrength = 0.8f;
    boundingRadius = 0.25f;
    addToGroup("torpedos");
    name = "torpedo";
    visible = false;

    selbstzerstoerungsTimer = new Timer();
    addChild(selbstzerstoerungsTimer);
}

void Torpedo::abfeuern(const glm::vec3& position, const glm::quat& rotation) {
    transform.position = position;
    transform.rotation = rotation;
    istAbgefeuert = true;
    visible = true;
    updateGlobalTransforms();
    selbstzerstoerungsTimer->startTimer(lebensdauer, [this]() { queueDestroy(); });
}

void Torpedo::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!istAbgefeuert) return;

    transform.position += transform.forward() * geschwindigkeit * deltaTime;

    kollisionPruefen();
}

void Torpedo::kollisionPruefen() {
    const auto& asteroiden = getNodesInGroup("asteroiden");
    for (auto* ziel : asteroiden) {
        float abstand = glm::distance(getGlobalTransform().position, ziel->getGlobalTransform().position);
        if (abstand < boundingRadius + ziel->boundingRadius) {
            if (parent != nullptr)
                parent->addChild(new Explosion(ziel->getGlobalTransform().position, 2.0f));
            ziel->queueDestroy();
            queueDestroy();
            return;
        }
    }

    const auto& feinde = getNodesInGroup("feinde");
    for (auto* ziel : feinde) {
        float abstand = glm::distance(getGlobalTransform().position, ziel->getGlobalTransform().position);
        if (abstand < boundingRadius + ziel->boundingRadius) {
            if (parent != nullptr)
                parent->addChild(new Explosion(ziel->getGlobalTransform().position, 4.0f));
            if (auto* schiff = dynamic_cast<Feindschiff*>(ziel)) {
                schiff->schadenNehmen(1.0f);
            } else {
                ziel->queueDestroy();
            }
            queueDestroy();
            return;
        }
    }
}
