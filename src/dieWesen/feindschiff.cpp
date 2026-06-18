#include "feindschiff.h"
#include "laser.h"

#include "../werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/random.h"

void Feindschiff::init() {
    loadModel("assets/models/spaceship2.obj");
    material.albedo = Kern::LoadTexture("assets/textures/feindschiff_albedo.png");
    material.roughness = Kern::LoadTexture("assets/textures/raumschiff_roughness.png");
    material.metallic = Kern::LoadTexture("assets/textures/raumschiff_metallic.png");
    material.normal = Kern::LoadTexture("assets/textures/raumschiff_normal.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    transform.scale = glm::vec3(5.0f);
    boundingRadius = 6.0f;
    addToGroup("feinde");
    name = "feindschiff";
    schussTimer = Random::range(0.0f, schussIntervall);
}

void Feindschiff::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (wegpunkte.empty()) return;

    const glm::vec3 ziel = wegpunkte[aktuellerWegpunkt];
    const glm::vec3 richtung = ziel - transform.position;
    const float abstand = glm::length(richtung);

    if (abstand < 2.0f) {
        aktuellerWegpunkt = (aktuellerWegpunkt + 1) % static_cast<int>(wegpunkte.size());
    } else {
        const glm::vec3 normRichtung = glm::normalize(richtung);
        const glm::quat zielRotation = Transform::quatLookAt(normRichtung, glm::vec3(0.0f, 1.0f, 0.0f));
        transform.rotation = glm::slerp(transform.rotation, zielRotation, drehGeschwindigkeit * deltaTime);
        transform.position += transform.forward() * bewegungsGeschwindigkeit * deltaTime;
    }

    schussTimer -= deltaTime;
    if (schussTimer <= 0.0f) {
        laserAbfeuern();
        schussTimer = schussIntervall;
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

void Feindschiff::schadenNehmen(float schaden) {
    leben -= schaden;
    if (leben <= 0.0f) {
        queueDestroy();
    }
}
