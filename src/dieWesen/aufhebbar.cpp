#include "aufhebbar.h"

#include "werkzeuge/input.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"
#include "werkzeuge/audio/audioManager.h"

Aufhebbar::Aufhebbar(GegenstandID gegenstandId, int anzahl)
    : gegenstandId(gegenstandId), anzahl(anzahl) {}

void Aufhebbar::init() {
    name = "aufhebbar";
    isCollidable = true;
    addToGroup("aufhebbar");

    const auto& info = GegenstandDaten::getInstance().getInfo(gegenstandId);
    if (!info.modellPfad.empty()) {
        loadModel(info.modellPfad.c_str());
        material.albedo = Kern::LoadTexture(info.modellAlbedoPfad.c_str());
        transform.scale = glm::vec3(0.3f);
    } else {
        loadModel("assets/models/cube.obj");
        material.albedo = info.iconTextur;
        transform.scale = glm::vec3(0.5f);
    }
    material.shader = ShaderManager::getInstance().getShader("default");
    startHoehe = transform.position.y;
}

void Aufhebbar::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (eingesammelt) return;

    bobZeit += deltaTime;
    transform.position.y = startHoehe + 0.3f * glm::sin(bobZeit * 2.0f);
    transform.rotation = glm::angleAxis(bobZeit * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));

    const auto& spieler = getNodesInGroup("spielerInsel");
    if (spieler.empty()) return;

    float abstand = glm::distance(
        getGlobalTransform().position,
        spieler[0]->getGlobalTransform().position
    );

    if (abstand < 3.0f && Input::isKeyJustPressed(GLFW_KEY_E)) {
        einsammeln();
    }
}

void Aufhebbar::einsammeln() {
    int rest = Inventar::getInstance().hinzufuegen(gegenstandId, anzahl);
    if (rest > 0) {
        anzahl = rest;
        return;
    }

    eingesammelt = true;
    AudioManager::getInstance().play2D("assets/audio/pickup.mp3", false, true);

    createTween()
        ->tweenProperty(&transform.scale.x, 0.0f, 0.2f, EaseType::EASE_IN_CUBIC)
        ->parallel()
        ->tweenProperty(&transform.scale.y, 0.0f, 0.2f, EaseType::EASE_IN_CUBIC)
        ->parallel()
        ->tweenProperty(&transform.scale.z, 0.0f, 0.2f, EaseType::EASE_IN_CUBIC)
        ->tweenCallback([this]() { queueDestroy(); });
}
