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

    if (istZielsuchend) {
        geschwindigkeit = 200.0f;
        lebensdauer = 5.0f;
        transform.scale = glm::vec3(0.2f, 0.2f, 3.0f);
        material.albedo = Kern::LoadTexture("assets/textures/emission_gruen.png");
        material.emission = Kern::LoadTexture("assets/textures/emission_gruen.png");
    }

    selbstzerstoerungsTimer->startTimer(lebensdauer, [this]() { queueDestroy(); });
}

void Torpedo::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!istAbgefeuert) return;

    if (istZielsuchend) {
        const auto& feinde = getNodesInGroup("feinde");
        glm::vec3 vorwaerts = transform.forward();
        Wesen* bestes = nullptr;
        float besteWertung = -0.5f;
        for (auto* f : feinde) {
            if (f->isQueuedDestroyed) continue;
            glm::vec3 richtung = f->getGlobalTransform().position - transform.position;
            float abstand = glm::length(richtung);
            if (abstand < 1.0f) continue;
            float dot = glm::dot(glm::normalize(richtung), vorwaerts);
            if (dot > besteWertung) {
                besteWertung = dot;
                bestes = f;
            }
        }

        if (bestes != nullptr) {
            glm::vec3 richtung = bestes->getGlobalTransform().position - transform.position;
            if (glm::length(richtung) > 1.0f) {
                glm::quat zielRotation = Transform::quatLookAt(glm::normalize(richtung), glm::vec3(0.0f, 1.0f, 0.0f));
                transform.rotation = glm::slerp(transform.rotation, zielRotation, 4.0f * deltaTime);
            }
        }
    }

    transform.position += transform.forward() * geschwindigkeit * deltaTime;
    kollisionPruefen();
}

void Torpedo::kollisionPruefen() {
    if (!istZielsuchend) {
        const auto& asteroiden = getNodesInGroup("asteroiden");
        for (auto* zielObj : asteroiden) {
            float abstand = glm::distance(getGlobalTransform().position, zielObj->getGlobalTransform().position);
            if (abstand < boundingRadius + zielObj->boundingRadius) {
                if (parent != nullptr)
                    parent->addChild(new Explosion(zielObj->getGlobalTransform().position, 2.0f));
                zielObj->queueDestroy();
                queueDestroy();
                return;
            }
        }
    }

    const auto& feinde = getNodesInGroup("feinde");
    for (auto* zielObj : feinde) {
        float abstand = glm::distance(getGlobalTransform().position, zielObj->getGlobalTransform().position);
        if (abstand < boundingRadius + zielObj->boundingRadius) {
            if (parent != nullptr)
                parent->addChild(new Explosion(zielObj->getGlobalTransform().position, 4.0f));
            if (auto* schiff = dynamic_cast<Feindschiff*>(zielObj)) {
                schiff->schadenNehmen(1.0f);
            } else {
                zielObj->queueDestroy();
            }
            queueDestroy();
            return;
        }
    }
}
