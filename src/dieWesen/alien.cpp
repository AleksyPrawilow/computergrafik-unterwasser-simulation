#include "alien.h"
#include "alienLaser.h"
#include "explosion.h"
#include "werkzeuge/random.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Alien::init() {
    loadModel("assets/models/xenomorph.obj");
    material.albedo = Kern::LoadTexture("assets/textures/xenomorph/T_MI_Xeno_Body_BaseColor.png");
    material.normal = Kern::LoadTexture("assets/textures/xenomorph/normalMap1.png");
    material.metallic = Kern::LoadTexture("assets/textures/xenomorph/metalnessMap1.png");
    material.bloomStrength = 0.0f;
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.scale = glm::vec3(1.0f);
    boundingRadius = 2.5f;
    bodenY = 1.5f;
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
            glm::quat blickRotation = Transform::quatLookAt(normRichtung, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat aufrechtKorrektur = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::quat zielRotation = blickRotation * aufrechtKorrektur;
            transform.rotation = glm::slerp(transform.rotation, zielRotation, 3.0f * deltaTime);
            transform.position += normRichtung * geschwindigkeit * deltaTime;
            transform.position.y = bodenY;
        }
    }

    schussTimer -= deltaTime;
    if (schussTimer <= 0.0f) {
        laserAbfeuern();
        schussTimer = schussIntervall;
    }
}

bool Alien::hatSichtlinie(const glm::vec3& von, const glm::vec3& nach) const {
    glm::vec3 richtung = nach - von;
    float strecke = glm::length(richtung);
    if (strecke < 0.01f) return true;
    glm::vec3 dir = richtung / strecke;

    const auto& waende = getNodesInGroup("hauswand");
    for (auto* wand : waende) {
        glm::mat4 invModel = glm::inverse(wand->getGlobalModelMatrix());
        glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(von, 1.0f));
        glm::vec3 localDir = glm::vec3(invModel * glm::vec4(dir, 0.0f));

        glm::vec3 bMin = wand->localAABB.min;
        glm::vec3 bMax = wand->localAABB.max;

        float tmin = 0.0f, tmax = strecke;
        for (int i = 0; i < 3; i++) {
            if (glm::abs(localDir[i]) < 0.0001f) {
                if (localOrigin[i] < bMin[i] || localOrigin[i] > bMax[i]) { tmin = strecke + 1.0f; break; }
            } else {
                float t1 = (bMin[i] - localOrigin[i]) / localDir[i];
                float t2 = (bMax[i] - localOrigin[i]) / localDir[i];
                if (t1 > t2) std::swap(t1, t2);
                tmin = glm::max(tmin, t1);
                tmax = glm::min(tmax, t2);
                if (tmin > tmax) break;
            }
        }
        if (tmin <= tmax && tmin < strecke) return false;
    }
    return true;
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

    if (!hatSichtlinie(pos, spielerPos)) return;

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
