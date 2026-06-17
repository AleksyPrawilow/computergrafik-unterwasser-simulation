//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "axe.h"

#include "player.h"
#include "timer.h"
#include "tree.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void Axe::init() {
    loadModel("assets/models/axe.obj");
    material.albedo = Kern::LoadTexture("assets/textures/axe_albedo.png");
    material.normal = Kern::LoadTexture("assets/textures/axe_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/axe_metallicRoughness.png");
    material.roughness = Kern::LoadTexture("assets/textures/axe_metallicRoughness.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.scale = glm::vec3(0.8f);
    transform.position = glm::vec3(0.75f, -0.3f, -1.25f);

    recoveryTimer = new Timer();
    addChild(recoveryTimer);

    hitSound = new AudioPlayer("assets/audio/tree_chop.mp3", false, 10.0f);
    addChild(hitSound);

    swingSound = new AudioPlayer("assets/audio/axe_swing.mp3", false, 10.0f);
    addChild(swingSound);
}

void Axe::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isPlayingAnimation && !recoveringAnimation) {
        isPlayingAnimation = true;
        swing();
    }
    transform.rotation = glm::quat(glm::radians(weaponEuler));

    if (treeToHit != nullptr) {
        treeToHit->hit(-hitNormal);
        treeToHit = nullptr;
    }
}

void Axe::swing() {
    const bool wasAwaitingRecovery = !recoveryTimer->active;
    recoveryTimer->stopTimer();
    Tween * tween = createTween();
    if (wasAwaitingRecovery) {
        tween
            ->tweenProperty(&transform.position.x, -0.08f, 0.38f, EaseType::EASE_OUT_BACK)
            ->parallel()
            ->tweenProperty(&weaponEuler.z, -35.0f, 0.25f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&weaponEuler.x, 20.0f, 0.25f, EaseType::EASE_OUT_CIRC)
            ->tweenInterval(0.05f);
    }
    tween
        ->tweenCallback([this]() {
            swingSound->play();
        })
        ->tweenProperty(&transform.position.x, 0.15f, 0.12f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&weaponEuler.z, -90.0f, 0.2f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&weaponEuler.x, -85.0f, 0.2f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenInterval(0.1f)
        ->parallel()
        ->tweenCallback([this]() {
            hitTree();
        })
        ->tweenInterval(0.15f)
        ->tweenProperty(&weaponEuler.z, -35.0f, 0.25f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&weaponEuler.x, 20.0f, 0.25f, EaseType::EASE_OUT_BACK)
        ->tweenCallback([this]() {
            isPlayingAnimation = false;
            recoveryTimer->startTimer(0.25f, [this]() { recoverAnimation(); });
        });
}

void Axe::recoverAnimation() {
    recoveringAnimation = true;
    createTween()
        ->tweenProperty(&transform.position.x, 0.75f, 0.5f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&weaponEuler.z, 0.0f, 0.35f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&weaponEuler.x, 0.0f, 0.35f, EaseType::EASE_OUT_BACK)
        ->tweenCallback([this]() {
            isPlayingAnimation = false;
            recoveringAnimation = false;
        });
}

void Axe::hitTree() {
    const auto * player = dynamic_cast<Player *>(parent);
    if (player == nullptr) {
        std::cerr << "Player is nullptr? Cos miales konkretnie zepsuc xD" << std::endl;
        return;
    }


    if (player->raycast->isColliding()) {
        if (const auto * hitObject = player->raycast->getCollider(); hitObject->name == "TreeHitbox") {
            auto * tree = dynamic_cast<Tree *>(hitObject->parent);
            treeToHit = tree;
            hitNormal = player->raycast->getCollisionNormal();
            hitSound->play();
            kamera.addShake(0.12f, 0.2f);
        }
    }
}
