#include "alien.h"
#include "alienLaser.h"
#include "explosion.h"
#include "werkzeuge/random.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Alien::init() {
    loadModel("assets/models/sphere.obj");
    material.albedo = Kern::LoadTexture("assets/textures/emission_lila.png");
    material.emission = Kern::LoadTexture("assets/textures/emission_lila.png");
    material.bloomStrength = 0.25f;
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.scale = glm::vec3(1.2f);
    boundingRadius = 1.5f;
    addToGroup("aliens");
    name = "alien";
    schussTimer = Random::range(0.0f, schussIntervall);
}

void Alien::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!wegpunkte.empty()) {
        glm::vec3 ziel = wegpunkte[aktuellerWegpunkt];
        ziel.y = bodenY;
        glm::vec3 richtung = ziel - transform.position;
        richtung.y = 0.0f;
        float abstand = glm::length(richtung);

        if (abstand < 1.0f) {
            aktuellerWegpunkt = (aktuellerWegpunkt + 1) % static_cast<int>(wegpunkte.size());
        } else {
            glm::vec3 normRichtung = glm::normalize(richtung);
            glm::quat zielRotation = Transform::quatLookAt(normRichtung, glm::vec3(0.0f, 1.0f, 0.0f));
            transform.rotation = glm::slerp(transform.rotation, zielRotation, 3.0f * deltaTime);
            transform.position += transform.forward() * geschwindigkeit * deltaTime;
            transform.position.y = bodenY;
        }
    }

    schussTimer -= deltaTime;
    if (schussTimer <= 0.0f) {
        laserAbfeuern();
        schussTimer = schussIntervall;
    }
}

void Alien::laserAbfeuern() {
    if (parent == nullptr) return;

    const auto& spieler = getNodesInGroup("spielerInsel");
    if (spieler.empty()) return;

    glm::vec3 spielerPos = spieler[0]->getGlobalTransform().position;
    glm::vec3 pos = getGlobalTransform().position;
    glm::vec3 richtung = spielerPos - pos;
    if (glm::length(richtung) < 1.0f) return;

    float abstand = glm::length(richtung);
    if (abstand > 40.0f) return;

    glm::quat schussRotation = Transform::quatLookAt(glm::normalize(richtung), glm::vec3(0.0f, 1.0f, 0.0f));

    auto* laser = new AlienLaser();
    parent->addChild(laser);
    laser->abfeuern(pos + glm::normalize(richtung) * 1.5f, schussRotation);
}

void Alien::schadenNehmen(float schaden) {
    leben -= schaden;
    if (leben <= 0.0f) {
        if (parent != nullptr) {
            parent->addChild(new Explosion(getGlobalTransform().position, 3.0f));
        }
        queueDestroy();
    }
}
