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
    axeNormal = Kern::LoadTexture("assets/textures/axe_normal.png");
    axeMetallic = Kern::LoadTexture("assets/textures/axe_metallicRoughness.png");
    axeRoughness = Kern::LoadTexture("assets/textures/axe_metallicRoughness.png");
    material.normal = axeNormal;
    material.metallic = axeMetallic;
    material.roughness = axeRoughness;
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.scale = glm::vec3(0.8f);
    transform.position = glm::vec3(0.75f, -0.3f, -1.25f);

    axeModelMesh = mesh;
    axeModelAABB = localAABB;

    CachedModel cubeModel = ModelManager::getInstance().getModel("assets/models/cube.obj");
    cubeModelMesh = cubeModel.mesh;
    cubeModelAABB = cubeModel.localAABB;

    CachedModel mapModel = ModelManager::getInstance().getModel("assets/models/map.obj");
    mapModelMesh = mapModel.mesh;
    mapModelAABB = mapModel.localAABB;
    mapAlbedo = Kern::LoadTexture("assets/textures/map.png");
    mapShader = ShaderManager::getInstance().loadShader(
        "map_fold", "assets/shaders/map_fold.vert", "assets/shaders/default.frag");

    CachedModel shovelModel = ModelManager::getInstance().getModel("assets/models/shovel.obj");
    shovelModelMesh = shovelModel.mesh;
    shovelModelAABB = shovelModel.localAABB;
    shovelAlbedo = Kern::LoadTexture("assets/textures/shovel_albedo.png");
    shovelNormal = Kern::LoadTexture("assets/textures/shovel_normal.png");
    shovelMetallic = Kern::LoadTexture("assets/textures/shovel_metallic.png");

    recoveryTimer = new Timer();
    addChild(recoveryTimer);

    hitSound = new AudioPlayer("assets/audio/tree_chop.mp3", false, 10.0f);
    addChild(hitSound);

    swingSound = new AudioPlayer("assets/audio/axe_swing.mp3", false, 10.0f);
    addChild(swingSound);

    shovelHitSound = new AudioPlayer("assets/audio/shovel.mp3", false, 10.0f);
    addChild(shovelHitSound);

    island = dynamic_cast<Island *>(getNodesInGroup("Island")[0]);

    visible = false;
}

void Axe::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    ausruestungAktualisieren();

    if (!visible) return;

    switch (letzteAktivesItem) {
        case GegenstandID::KARTE:
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !karteOffen && !karteAnimiert) {
                karteAnimiert = true;
                karteOffen = true;
                createTween()
                    ->tweenProperty(&transform.position, karteLesePos, 0.4f, EaseType::EASE_OUT_CUBIC)
                    ->parallel()
                    ->tweenProperty(&weaponEuler, karteLeseEuler, 0.4f, EaseType::EASE_OUT_CUBIC)
                    ->tweenCallback([this]() { karteAnimiert = false; });
            } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS && karteOffen && !karteAnimiert) {
                karteAnimiert = true;
                karteOffen = false;
                createTween()
                    ->tweenProperty(&transform.position, karteIdlePos, 0.3f, EaseType::EASE_OUT_CUBIC)
                    ->parallel()
                    ->tweenProperty(&weaponEuler, karteIdleEuler, 0.3f, EaseType::EASE_OUT_CUBIC)
                    ->tweenCallback([this]() { karteAnimiert = false; });
            }
            break;
        case GegenstandID::AXT:
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isPlayingAnimation && !recoveringAnimation) {
                isPlayingAnimation = true;
                swing();
            }
            break;
        case GegenstandID::HOLZAXT:
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isPlayingAnimation && !recoveringAnimation) {
                isPlayingAnimation = true;
                swing();
            }
            break;
        case GegenstandID::SCHAUFEL:
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isPlayingAnimation && !recoveringAnimation) {
                isPlayingAnimation = true;
                shovelDig();
            }
            break;
    }

    transform.rotation = glm::quat(glm::radians(weaponEuler));

    if (treeToHit != nullptr) {
        GegenstandID aktiv = Inventar::getInstance().getAktivesItem();
        const auto& info = GegenstandDaten::getInstance().getInfo(aktiv);
        int schaden = (info.werkzeugSchaden > 0) ? info.werkzeugSchaden : 1;
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
        karteOffen = false;
        karteAnimiert = false;
        return;
    }

    karteOffen = false;
    karteAnimiert = false;
    weaponEuler = glm::vec3(0.0f);

    visible = true;
    const auto& info = GegenstandDaten::getInstance().getInfo(aktiv);

    if (info.werkzeugTyp == WerkzeugTyp::AXT) {
        mesh = axeModelMesh;
        localAABB = axeModelAABB;
        material.albedo = (aktiv == GegenstandID::HOLZAXT) ? defaultAlbedo : upgradedAlbedo;
        material.normal = axeNormal;
        material.roughness = axeRoughness;
        material.metallic = axeMetallic;
        material.shader = ShaderManager::getInstance().getShader("default");
        transform.scale = glm::vec3(0.8f);
        transform.position = glm::vec3(0.75f, -0.3f, -1.25f);
        istAxt = true;
    } else if (aktiv == GegenstandID::KARTE) {
        mesh = mapModelMesh;
        localAABB = mapModelAABB;
        material.albedo = mapAlbedo;
        material.normal = 0;
        material.roughness = 0;
        material.metallic = 0;
        material.shader = mapShader;
        transform.scale = glm::vec3(0.4f);
        transform.position = karteIdlePos;
        weaponEuler = glm::vec3(0.0f);
        istAxt = false;
    } else if (aktiv == GegenstandID::SCHAUFEL) {
        mesh = shovelModelMesh;
        localAABB = shovelModelAABB;
        material.albedo = shovelAlbedo;
        material.normal = shovelNormal;
        material.roughness = shovelMetallic;
        material.metallic = shovelMetallic;
        material.shader = ShaderManager::getInstance().getShader("default");
        weaponEuler = glm::vec3(0.0f);
        transform.scale = glm::vec3(0.8f);
        transform.position = glm::vec3(0.75f, -0.3f, -1.25f);
        istAxt = false;
    } else if (!info.modellPfad.empty()) {
        CachedModel custom = ModelManager::getInstance().getModel(info.modellPfad);
        mesh = custom.mesh;
        localAABB = custom.localAABB;
        material.albedo = Kern::LoadTexture(info.modellAlbedoPfad.c_str());
        material.normal = 0;
        material.roughness = 0;
        material.metallic = 0;
        material.shader = ShaderManager::getInstance().getShader("default");
        transform.scale = glm::vec3(0.15f);
        transform.position = glm::vec3(0.5f, -0.3f, -1.0f);
        istAxt = false;
    } else {
        mesh = cubeModelMesh;
        localAABB = cubeModelAABB;
        material.albedo = info.iconTextur;
        material.normal = 0;
        material.roughness = 0;
        material.metallic = 0;
        material.shader = ShaderManager::getInstance().getShader("default");
        transform.scale = glm::vec3(0.3f, 0.3f, 0.03f);
        transform.position = glm::vec3(0.75f, -0.3f, -1.25f);
        istAxt = false;
    }
}

void Axe::prepareUniforms() const {
    if (letzteAktivesItem == GegenstandID::KARTE) {
        Kern::setUniform(material.shader, "u_progress", 1.0f);
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

void Axe::hitGround() const {
    shovelHitSound->play();
    island->graben(parent->getGlobalTransform().position, 2.0f, 1.5f);
}

void Axe::shovelDig() {
    const bool wasAwaitingRecovery = !recoveryTimer->active;
    recoveryTimer->stopTimer();
    Tween* tween = createTween();

    if (wasAwaitingRecovery) {
        tween
            ->tweenProperty(&transform.position.x, 0.75f, 0.28f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&transform.position.z, -0.80f, 0.28f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&transform.position.y, -0.15f, 0.28f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&weaponEuler.x, -10.0f, 0.28f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&weaponEuler.y, 15.0f, 0.28f, EaseType::EASE_OUT_SINE)
            ->tweenInterval(0.04f);
    }

    tween
        ->tweenCallback([this]() {
            swingSound->play();
        })
        ->tweenProperty(&transform.position.x, 0.20f, 0.16f, EaseType::EASE_IN)
        ->parallel()
        ->tweenProperty(&transform.position.z, -1.50f, 0.16f, EaseType::EASE_IN)
        ->parallel()
        ->tweenProperty(&transform.position.y, -0.85f, 0.16f, EaseType::EASE_IN)
        ->parallel()
        ->tweenProperty(&weaponEuler.x, -45.0f, 0.16f, EaseType::EASE_IN)
        ->parallel()
        ->tweenProperty(&weaponEuler.y, -10.0f, 0.16f, EaseType::EASE_IN)

        ->tweenCallback([this]() {
            hitGround();
        })
        ->tweenProperty(&transform.position.x, -0.15f, 0.22f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&transform.position.z, -1.65f, 0.22f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&transform.position.y, -0.10f, 0.22f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&weaponEuler.x, 40.0f, 0.25f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&weaponEuler.y, -25.0f, 0.25f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&weaponEuler.z, -45.0f, 0.22f, EaseType::EASE_OUT_CUBIC)
        ->tweenInterval(0.08f)

        ->tweenProperty(&transform.position.x, 0.45f, 0.35f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&transform.position.z, -1.15f, 0.35f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&transform.position.y, -0.40f, 0.35f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&weaponEuler.x, 0.0f, 0.35f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&weaponEuler.y, 0.0f, 0.35f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&weaponEuler.z, 0.0f, 0.35f, EaseType::EASE_OUT_SINE)

        ->tweenCallback([this]() {
            isPlayingAnimation = false;
            recoveryTimer->startTimer(0.25f, [this]() { recoverAnimation(); });
        });
}
void Axe::shovelRecover() {
    recoveringAnimation = true;

    createTween()
        ->tweenProperty(&transform.position.x, 0.75f, 0.5f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&transform.position.y, -0.4f, 0.5f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&transform.position.z, -1.25f, 0.5f, EaseType::EASE_OUT_BACK)

        ->parallel()
        ->tweenProperty(&weaponEuler.x, 0.0f, 0.35f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&weaponEuler.z, 0.0f, 0.35f, EaseType::EASE_OUT_BACK)

        ->tweenCallback([this]() {
            isPlayingAnimation = false;
            recoveringAnimation = false;
        });
}
