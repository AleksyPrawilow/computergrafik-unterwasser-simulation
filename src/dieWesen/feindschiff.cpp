#include "feindschiff.h"
#include "explosion.h"
#include "feuerEmitter.h"
#include "laser.h"
#include "schildPickup.h"

#include "../werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/random.h"
#include "szenes/weltraumszeneWesen.h"

void Feindschiff::init() {
    loadModel("assets/models/enemy_spaceship.obj");
    material.albedo = Kern::LoadTexture("assets/textures/enemy_spaceship/Raven_sketchfablambert5SG.png");
    material.emission = Kern::LoadTexture("assets/textures/enemy_spaceship/emissiveMap1.png");
    material.metallic = Kern::LoadTexture("assets/textures/enemy_spaceship/metalnessMap1.png");
    material.normal = Kern::LoadTexture("assets/textures/enemy_spaceship/normalMap1.png");
    material.bloomStrength = 0.1f;
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    transform.scale = glm::vec3(6.0f);
    boundingRadius = 7.2f;
    addToGroup("feinde");
    name = "feindschiff";
    schussTimer = Random::range(0.0f, schussIntervall);
    orbitWinkel = Random::range(0.0f, 6.28f);
    orbitRadius = Random::range(35.0f, 55.0f);
}

Wesen* Feindschiff::findeSpieler() const {
    const auto& spieler = getNodesInGroup("spieler");
    return spieler.empty() ? nullptr : spieler[0];
}

void Feindschiff::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (istTot) {
        transform.roll(0.3f * deltaTime);
        transform.pitch(0.1f * deltaTime);
        transform.position += transform.forward() * 2.0f * deltaTime;

        feuerTimer -= deltaTime;
        if (feuerTimer <= 0.0f) {
            feuerTimer = 0.3f;
            if (parent != nullptr) {
                glm::vec3 pos = getGlobalTransform().position;
                glm::vec3 offset(
                    Random::range(-transform.scale.x * 0.3f, transform.scale.x * 0.3f),
                    Random::range(-transform.scale.y * 0.2f, transform.scale.y * 0.2f),
                    Random::range(-transform.scale.z * 0.3f, transform.scale.z * 0.3f)
                );
                parent->addChild(new Explosion(pos + offset, Random::range(3.0f, 8.0f)));
            }
        }
        return;
    }

    switch (typ) {
        case FeindTyp::PATROUILLE: updatePatrouille(deltaTime); break;
        case FeindTyp::JAEGER:     updateJaeger(deltaTime); break;
        case FeindTyp::ORBITER:    updateOrbiter(deltaTime); break;
    }

    schussTimer -= deltaTime;
    if (schussTimer <= 0.0f) {
        if (typ == FeindTyp::PATROUILLE) {
            laserAbfeuern();
        } else {
            auto* spieler = findeSpieler();
            if (spieler) {
                laserAbfeuernAufZiel(spieler->getGlobalTransform().position);
            } else {
                laserAbfeuern();
            }
        }
        schussTimer = schussIntervall;
    }
}

void Feindschiff::updatePatrouille(float deltaTime) {
    if (wegpunkte.empty()) return;

    const glm::vec3 ziel = wegpunkte[aktuellerWegpunkt];
    const glm::vec3 richtung = ziel - transform.position;
    const float abstand = glm::length(richtung);

    if (abstand < 2.0f) {
        aktuellerWegpunkt = (aktuellerWegpunkt + 1) % static_cast<int>(wegpunkte.size());
    } else {
        glm::vec3 blickZiel = glm::normalize(richtung);

        auto* spieler = findeSpieler();
        if (spieler) {
            glm::vec3 zumSpieler = spieler->getGlobalTransform().position - transform.position;
            float spielerAbstand = glm::length(zumSpieler);
            if (spielerAbstand < 120.0f && spielerAbstand > 1.0f) {
                blickZiel = glm::mix(blickZiel, glm::normalize(zumSpieler), 0.3f);
                blickZiel = glm::normalize(blickZiel);
            }
        }

        const glm::quat zielRotation = Transform::quatLookAt(blickZiel, glm::vec3(0.0f, 1.0f, 0.0f));
        transform.rotation = glm::slerp(transform.rotation, zielRotation, drehGeschwindigkeit * deltaTime);
        transform.position += transform.forward() * bewegungsGeschwindigkeit * deltaTime;
    }
}

void Feindschiff::updateJaeger(float deltaTime) {
    auto* spieler = findeSpieler();
    if (!spieler) { updatePatrouille(deltaTime); return; }

    glm::vec3 spielerPos = spieler->getGlobalTransform().position;
    glm::vec3 richtung = spielerPos - transform.position;
    float abstand = glm::length(richtung);

    if (abstand > 1.0f) {
        glm::vec3 normRichtung = glm::normalize(richtung);
        glm::quat zielRotation = Transform::quatLookAt(normRichtung, glm::vec3(0.0f, 1.0f, 0.0f));
        transform.rotation = glm::slerp(transform.rotation, zielRotation, drehGeschwindigkeit * 0.4f * deltaTime);

        float geschwindigkeit = bewegungsGeschwindigkeit * 2.0f;
        if (abstand < 30.0f) geschwindigkeit *= 0.5f;
        transform.position += transform.forward() * geschwindigkeit * deltaTime;
    }
}

void Feindschiff::updateOrbiter(float deltaTime) {
    auto* spieler = findeSpieler();
    if (!spieler) { updatePatrouille(deltaTime); return; }

    glm::vec3 spielerPos = spieler->getGlobalTransform().position;

    float orbitGeschwindigkeit = bewegungsGeschwindigkeit * 3.0f / orbitRadius;
    orbitWinkel += orbitGeschwindigkeit * deltaTime;

    glm::vec3 zielPos = spielerPos + glm::vec3(
        glm::cos(orbitWinkel) * orbitRadius,
        glm::sin(orbitWinkel * 0.5f) * 20.0f,
        glm::sin(orbitWinkel) * orbitRadius
    );

    glm::vec3 richtung = zielPos - transform.position;
    float abstand = glm::length(richtung);
    if (abstand > 1.0f) {
        float annaeherung = glm::min(bewegungsGeschwindigkeit * 3.0f, abstand / deltaTime);
        transform.position += glm::normalize(richtung) * annaeherung * deltaTime;
    }

    glm::vec3 blickRichtung = spielerPos - transform.position;
    if (glm::length(blickRichtung) > 1.0f) {
        glm::quat zielRotation = Transform::quatLookAt(glm::normalize(blickRichtung), glm::vec3(0.0f, 1.0f, 0.0f));
        transform.rotation = glm::slerp(transform.rotation, zielRotation, drehGeschwindigkeit * 1.5f * deltaTime);
    }
}

void Feindschiff::laserAbfeuern() {
    if (parent == nullptr) return;

    const glm::vec3 vorwaerts = transform.forward();
    const glm::vec3 rechts = transform.right();
    const glm::vec3 pos = getGlobalTransform().position;

    float fwd = transform.scale.x * 0.6f;
    for (int i = 0; i < 2; i++) {
        auto * laser = new Laser();
        laser->schaden = laserSchaden;
        float seite = (i == 0) ? -1.0f : 1.0f;
        parent->addChild(laser);
        laser->transform.scale = laserGroesse;
        laser->abfeuern(pos + vorwaerts * fwd + rechts * seite * laserOffset, transform.rotation);
    }
}

void Feindschiff::laserAbfeuernAufZiel(const glm::vec3& zielPos) {
    if (parent == nullptr) return;

    const glm::vec3 pos = getGlobalTransform().position;
    const glm::vec3 rechts = transform.right();
    glm::vec3 schussRichtung = zielPos - pos;
    if (glm::length(schussRichtung) < 1.0f) return;
    schussRichtung = glm::normalize(schussRichtung);

    glm::quat schussRotation = Transform::quatLookAt(schussRichtung, glm::vec3(0.0f, 1.0f, 0.0f));

    float fwd = transform.scale.x * 0.6f;
    for (int i = 0; i < 2; i++) {
        auto * laser = new Laser();
        laser->schaden = laserSchaden;
        float seite = (i == 0) ? -1.0f : 1.0f;
        parent->addChild(laser);
        laser->transform.scale = laserGroesse;
        laser->abfeuern(pos + schussRichtung * fwd + rechts * seite * laserOffset, schussRotation);
    }
}

void Feindschiff::schadenNehmen(float schaden) {
    if (istTot) return;
    leben -= schaden;
    if (leben <= 0.0f) {
        if (parent != nullptr) {
            float explosionsGroesse = glm::max(transform.scale.x * 0.8f, 5.0f);
            parent->addChild(new Explosion(getGlobalTransform().position, explosionsGroesse));
            if (Random::range(0.0f, 1.0f) < 0.4f) {
                parent->addChild(new SchildPickup(getGlobalTransform().position));
            }
        }
        WeltraumszeneWesen::abschussZaehlen();

        bool istBoss = transform.scale.x > 50.0f;
        if (istBoss) {
            istTot = true;
            removeFromGroup("feinde");
            addToGroup("bossLeiche");
            feuerTimer = 0.0f;

            float bossStreuung = transform.scale.x * 0.5f;
            for (int i = 0; i < 5; i++) {
                auto* feuer = new FeuerEmitter(800, bossStreuung);
                feuer->active = true;
                feuer->zielObjekt = this;
                feuer->offset = glm::vec3(
                    Random::range(-bossStreuung * 0.2f, bossStreuung * 0.2f),
                    Random::range(-bossStreuung * 0.1f, bossStreuung * 0.1f),
                    Random::range(-bossStreuung * 0.2f, bossStreuung * 0.2f)
                );
                feuer->transform.position = getGlobalTransform().position + feuer->offset;
                parent->addChild(feuer);
                feuer->addToGroup("bossLeiche");
            }
        } else {
            queueDestroy();
        }
    }
}
