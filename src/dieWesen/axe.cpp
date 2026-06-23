//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "axe.h"

#include "player.h"
#include "timer.h"
#include "tree.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void Axe::init() {
    loadModel("assets/models/axe.obj");
    defaultAlbedo = Kern::LoadTexture("assets/textures/axe_albedo.png");
    upgradedAlbedo = Kern::LoadTexture("assets/textures/icon_axt.png");
    material.albedo = defaultAlbedo;
    material.normal = Kern::LoadTexture("assets/textures/axe_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/axe_metallicRoughness.png");
    material.roughness = Kern::LoadTexture("assets/textures/axe_metallicRoughness.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.scale = glm::vec3(0.8f);
    transform.position = glm::vec3(0.75f, -0.3f, -1.25f);

    axeModelMesh = mesh;
    axeModelAABB = localAABB;

    CachedModel cubeModel = ModelManager::getInstance().getModel("assets/models/cube.obj");
    cubeModelMesh = cubeModel.mesh;
    cubeModelAABB = cubeModel.localAABB;

    recoveryTimer = new Timer();
    addChild(recoveryTimer);

    hitSound = new AudioPlayer("assets/audio/tree_chop.mp3", false, 10.0f);
    addChild(hitSound);

    swingSound = new AudioPlayer("assets/audio/axe_swing.mp3", false, 10.0f);
    addChild(swingSound);

    visible = false;
}

void Axe::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    ausruestungAktualisieren();

    if (!visible) return;

    if (istAxt && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isPlayingAnimation && !recoveringAnimation) {
        isPlayingAnimation = true;
        swing();
    }
    transform.rotation = glm::quat(glm::radians(weaponEuler));

    if (treeToHit != nullptr) {
        GegenstandID aktiv = Inventar::getInstance().getAktivesItem();
        int schaden = (aktiv == GegenstandID::AXT) ? 5 : 1;
        treeToHit->hit(-hitNormal, schaden);
        treeToHit = nullptr;
    }
}

void Axe::ausruestungAktualisieren() {
    GegenstandID aktiv = Inventar::getInstance().getAktivesItem();

    if (aktiv == letzteAktivesItem) return;
    letzteAktivesItem = aktiv;

    if (aktiv == GegenstandID::KEINE) {
        visible = false;
        istAxt = false;
        return;
    }

    visible = true;

    if (aktiv == GegenstandID::HOLZAXT) {
        mesh = axeModelMesh;
        localAABB = axeModelAABB;
        material.albedo = defaultAlbedo;
        transform.scale = glm::vec3(0.8f);
        istAxt = true;
    } else if (aktiv == GegenstandID::AXT) {
        mesh = axeModelMesh;
        localAABB = axeModelAABB;
        material.albedo = upgradedAlbedo;
        transform.scale = glm::vec3(0.8f);
        istAxt = true;
    } else {
        const auto& info = GegenstandDaten::getInstance().getInfo(aktiv);
        mesh = cubeModelMesh;
        localAABB = cubeModelAABB;
        material.albedo = info.iconTextur;
        transform.scale = glm::vec3(0.3f, 0.3f, 0.03f);
        istAxt = false;
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
        return;
    }

    if (player->raycast->isColliding()) {
        if (auto * hitObject = player->raycast->getCollider(); hitObject->name == "tree") {
            auto * tree = dynamic_cast<Tree *>(hitObject);
            treeToHit = tree;
            hitSound->play();
            kamera.addShake(0.12f, 0.2f);
        }
    }
}
