#include "vergrabeneSchatztruhe.h"

#include "aufhebbar.h"
#include "player.h"
#include "werkzeuge/input.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/visual/questManager.h"
#include "werkzeuge/renderWerkzeuge.h"

extern Kamera kamera;

void VergrabeneSchatztruhe::init() {
    name = "schatztruhe";
    isCollidable = true;
    addToGroup("schatztruhe");

    loadModel("assets/models/cube.obj");
    material.albedo = Kern::LoadTexture("assets/textures/icon_truhe.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.scale = glm::vec3(1.2f, 0.8f, 0.8f);
    visible = false;

    std::cout << "[Schatz] Chest spawned at (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")" << std::endl;

    interactLabel = new UILabel();
    interactLabel->text = "[E] Open chest";
    interactLabel->fontSize = 36.0f;
    interactLabel->expansion = UIExpansion::CENTER;
    interactLabel->visible = false;
}

void VergrabeneSchatztruhe::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (geoeffnet) return;

    if (freigelegt && !geoeffnet) {
        const auto& spieler = getNodesInGroup("spielerInsel");
        if (spieler.empty()) return;

        float abstand = glm::distance(
            getGlobalTransform().position,
            spieler[0]->getGlobalTransform().position
        );

        if (abstand < 4.0f) {
            if (!interactLabel->visible && parent != nullptr) {
                glm::vec2 viewport = Kern::GetViewportSize();
                interactLabel->transform.position = glm::vec3(viewport.x * 0.5f, viewport.y * 0.5f + 50.0f, 0.0f);
                parent->addChild(interactLabel);
                interactLabel->visible = true;
            } else {
                interactLabel->visible = true;
            }

            if (Input::isKeyJustPressed(GLFW_KEY_E)) {
                oeffnen();
            }
        } else {
            interactLabel->visible = false;
        }
    }
}

void VergrabeneSchatztruhe::graben() {
    if (freigelegt) return;

    grabFortschritt++;
    std::cout << "[Schatz] Dig progress: " << grabFortschritt << "/" << GRABEN_BENOETIGT << std::endl;

    if (grabFortschritt >= GRABEN_BENOETIGT) {
        freigelegt = true;
        visible = true;
        markierung->visible = false;

        createTween()
            ->tweenProperty(&transform.position.y, transform.position.y + 1.0f, 0.5f, EaseType::EASE_OUT_BACK);

        QuestManager::getInstance().progressObjective("dig_out", 1);
    }
}

void VergrabeneSchatztruhe::oeffnen() {
    if (geoeffnet) return;
    geoeffnet = true;
    interactLabel->visible = false;

    kamera.addShake(0.2f, 0.3f);
    AudioManager::getInstance().play2D("assets/audio/quest_complete.wav", false, true);

    createTween()
        ->tweenProperty(&transform.scale.y, 0.4f, 0.3f, EaseType::EASE_OUT_CUBIC)
        ->tweenCallback([this]() {
            if (parent == nullptr) return;

            auto* miniUboot = new Aufhebbar(GegenstandID::MINIUBOOT, 1);
            miniUboot->transform.position = getGlobalTransform().position;
            miniUboot->transform.position.y += 1.5f;
            parent->addChild(miniUboot);
        });
}
