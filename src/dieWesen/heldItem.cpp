//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "heldItem.h"

#include "player.h"
#include "thunderstorm.h"
#include "timer.h"
#include "tree.h"
#include "uboot.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/musicManager.h"
#include "werkzeuge/visual/questManager.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void HeldItem::init() {
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

    const auto& inseln = getNodesInGroup("Island");
    island = inseln.empty() ? nullptr : dynamic_cast<Island *>(inseln[0]);

    visible = false;
}

void HeldItem::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
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
    case GegenstandID::MINIUBOOT:
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isPlayingAnimation && !recoveringAnimation) {
                isPlayingAnimation = false;
                throwSub();
            }
            break;
    case GegenstandID::FLOSS:
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isPlayingAnimation && !recoveringAnimation) {
                isPlayingAnimation = false;
                throwRaft();
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

void HeldItem::ausruestungAktualisieren() {
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
    } else if (info.werkzeugTyp == WerkzeugTyp::ANGEL) {
        mesh = cubeModelMesh;
        localAABB = cubeModelAABB;
        material.albedo = Kern::LoadTexture("assets/textures/Raft_baseColor.png");
        material.normal = 0;
        material.roughness = 0;
        material.metallic = 0;
        material.shader = ShaderManager::getInstance().getShader("default");
        transform.scale = glm::vec3(0.04f, 0.04f, 1.2f);
        transform.position = glm::vec3(0.4f, -0.2f, -0.8f);
        weaponEuler = glm::vec3(15.0f, 0.0f, 0.0f);
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

void HeldItem::prepareUniforms() const {
    if (letzteAktivesItem == GegenstandID::KARTE) {
        Kern::setUniform(material.shader, "u_progress", 1.0f);
    }
}

void HeldItem::swing() {
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

void HeldItem::recoverAnimation() {
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

void HeldItem::hitTree() {
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

void HeldItem::hitGround() const {
    shovelHitSound->play();
    if (island != nullptr) {
        island->graben(parent->getGlobalTransform().position, 4.0f, 1.5f);
    }
}

void HeldItem::shovelDig() {
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
void HeldItem::shovelRecover() {
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

void HeldItem::throwRaft() {
    auto * raft = new Raft();
    parent->parent->addChild(raft);
    raft->transform = getGlobalTransform();
    raft->transform.scale = glm::vec3(0.1f);
    raft->shouldFloat = false;
    raft->player = dynamic_cast<Player *>(parent);

    float throwDistance = 15.0f;
    glm::vec3 startPos = getGlobalTransform().position;
    glm::vec3 worldForward = getGlobalTransform().forward();
    glm::vec3 targetPos = startPos + worldForward * throwDistance;
    targetPos.y = 0.0f;

    int targetSlotIndex = Inventar::getInstance().getAktiverSlot();
    Inventar::getInstance().hotbarVerbrauchen(targetSlotIndex);
    createTween()
        ->tweenProperty(&raft->transform.position.x, targetPos.x, 1.0f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&raft->transform.position.z, targetPos.z, 1.0f, EaseType::EASE_OUT_CUBIC);
    createTween()
        ->tweenProperty(&raft->transform.position.y, startPos.y + 2.0f, 0.4f, EaseType::EASE_OUT)
        ->tweenProperty(&raft->transform.position.y, targetPos.y, 0.4f, EaseType::EASE_IN)
        ->tweenProperty(&raft->transform.scale, glm::vec3(0.5f), 0.3f, EaseType::EASE_OUT_BOUNCE)
        ->tweenCallback([raft]() {
            raft->shouldFloat = true;
            raft->addToGroup("player");
        });
}

void HeldItem::throwSub() {
    MusicManager::getInstance().playMusic("assets/audio/sub_intro.mp3", 2.0f);
    auto * uboot = new Uboot();
    parent->parent->addChild(uboot);
    uboot->transform = getGlobalTransform();
    uboot->shouldFloat = false;
    uboot->setIsActive(false);
    uboot->player = dynamic_cast<Player *>(parent);

    float throwDistance = 20.0f;
    glm::vec3 startPos = getGlobalTransform().position;
    glm::vec3 worldForward = getGlobalTransform().forward();
    glm::vec3 targetPos = startPos + worldForward * throwDistance;
    targetPos.y = 0.0f;

    int targetSlotIndex = Inventar::getInstance().getAktiverSlot();
    Inventar::getInstance().hotbarVerbrauchen(targetSlotIndex);
    createTween()
        ->tweenProperty(&uboot->transform.position.x, targetPos.x, 1.2f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&uboot->transform.position.z, targetPos.z, 1.2f, EaseType::EASE_OUT_CUBIC);
    createTween()
        ->tweenProperty(&uboot->transform.position.y, startPos.y + 2.5f, 0.5f, EaseType::EASE_OUT)
        ->tweenProperty(&uboot->transform.position.y, targetPos.y, 0.5f, EaseType::EASE_IN)
        ->tweenCallback([this, uboot]() {
            QuestManager::getInstance().progressObjective("submarine_throw");
            uboot->shouldFloat = true;
        })
        ->tweenInterval(1.0f)
        ->tweenCallback([this, targetPos]() {
            auto * thunderstorm = dynamic_cast<Thunderstorm * >(getNodesInGroup("thunderstorm")[0]);
            thunderstorm->triggerLightning(targetPos, 2);
        })
        ->tweenProperty(&uboot->transform.scale, glm::vec3(3.0f), 0.1f, EaseType::EASE_OUT_BOUNCE)
        ->tweenInterval(1.0f)
        ->tweenCallback([this, uboot]() {
            showcaseSub(uboot);
        });
}

void HeldItem::showcaseSub(Uboot * uboot) {
    dynamic_cast<Player * >(parent)->setActive(false);
    kamera.transform.position = uboot->getGlobalTransform().position - uboot->transform.right() * 2.0f - uboot->transform.forward() * 4.0f;
    kamera.transform.lookAt(uboot->transform.position - uboot->transform.forward() * 4.0f);
    auto stage2pos = uboot->getGlobalTransform().position + uboot->transform.up() * 2.0f + uboot->transform.forward() * 4.0f;
    auto stage3pos = uboot->getGlobalTransform().position + uboot->transform.forward() * 8.0f;
    createTween()
        ->tweenInterval(0.5f)
        ->tweenProperty(&kamera.transform.position, kamera.transform.position + uboot->transform.forward() * 4.0f, 3.0f, EaseType::LINEAR)
        ->tweenCallback([this, uboot]() {
            kamera.transform.position = uboot->getGlobalTransform().position + uboot->transform.up() * 2.0f;
            kamera.transform.lookAt(uboot->transform.position);
        })
        ->tweenProperty(&kamera.transform.position, stage2pos, 3.0f, EaseType::LINEAR)
        ->tweenCallback([this, uboot]() {
            kamera.transform.position = uboot->getGlobalTransform().position + uboot->transform.forward() * 4.0f;
            kamera.transform.lookAt(uboot->transform.position);
        })
        ->tweenProperty(&kamera.transform.position, stage3pos, 3.0f, EaseType::EASE_OUT_SINE)
        ->tweenInterval(0.5f)
        ->tweenCallback([this, uboot]() {
            dynamic_cast<Player * >(parent)->setActive(true);
            uboot->addToGroup("player");
            MusicManager::getInstance().playMusic("assets/audio/dramatic.mp3");
            auto * thunderstorm = dynamic_cast<Thunderstorm * >(getNodesInGroup("thunderstorm")[0]);
            thunderstorm->beginThunderstorm();
        });
}
