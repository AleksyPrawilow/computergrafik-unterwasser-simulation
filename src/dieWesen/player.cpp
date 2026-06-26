//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "player.h"
#include "uboot.h"
#include "heldItem.h"
#include "aufhebbar.h"
#include "grabLoch.h"
#include "platzierbaresObjekt.h"
#include "spielerLaser.h"
#include "ui/fadenkreuz.h"
#include "werkzeuge/input.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/visual/questManager.h"

extern Kamera kamera;

void Player::init() {
    transform.scale = glm::vec3(1.0f);
    transform.position = glm::vec3(-700.0f, 0.0f, -220.0f);
    addToGroup("spielerInsel");
    const auto& inseln = getNodesInGroup("Island");
    island = inseln.empty() ? nullptr : dynamic_cast<Island *>(inseln[0]);
    auto * crosshair = new Fadenkreuz();
    crosshair->init(16.0f / 7.0f);
    addChild(crosshair);

    auto * heldItem = new HeldItem();
    addChild(heldItem);

    rayTargetHelper = new Wesen();
    rayTargetHelper->loadModel("assets/models/hitPoint.obj");
    rayTargetHelper->material.shader = ShaderManager::getInstance().getShader("default");
    rayTargetHelper->material.opacity = Kern::LoadTexture("assets/textures/jellyfish_opacity.png");
    rayTargetHelper->material.albedo = Kern::LoadTexture("assets/textures/jellyfish_inner_albedo.png");
    rayTargetHelper->material.isTransparent = true;
    rayTargetHelper->transform.scale = glm::vec3(0.25f);
    parent->addChild(rayTargetHelper);

    raycast = new RayCast();
    raycast->targetPosition = glm::vec3(0.0f, 0.0f, -3.5f);
    addChild(raycast);

    raycastTimer = new Timer();
    addChild(raycastTimer);
    raycastTimer->startTimer(0.1f, [this]() { updateRaycast(); });

    schussTimer = new Timer();
    addChild(schussTimer);

    rollTimer = new Timer();
    addChild(rollTimer);

    addToGroup("playerWalking");
}

void Player::onUpdate(GLFWwindow* window, const float deltaTime, Transform& cameraTransform) {
    if (buffTimer > 0.0f) {
        buffTimer -= deltaTime;
        if (buffTimer <= 0.0f) {
            buffTimer = 0.0f;
            speedMultiplier = 1.0f;
            jumpMultiplier = 1.0f;
        }
    }
    if (spawnSchutz > 0.0f) spawnSchutz -= deltaTime;
    if (schadenBlitz > 0.0f) schadenBlitz -= deltaTime * 3.0f;

    if (istRollend) {
        rollZeit += deltaTime;
        float t = rollZeit / rollDauer;
        if (t >= 1.0f) {
            istRollend = false;
            rollWinkel = 0.0f;
            grounded = true;
            verticalVelocity = 0.0f;
            transform.position.y = targetY;
            rollAbklingzeit = true;
            rollTimer->startTimer(0.3f, [this]() { rollAbklingzeit = false; });
        } else {
            float geschw = rollGeschwindigkeit * rollSprintMult * (1.0f - t * 0.5f);
            glm::vec3 oldPos = transform.position;
            transform.position += rollRichtung * geschw * deltaTime;
            transform.position.y = targetY;
            rollWinkel = glm::sin(t * 3.14159f) * 35.0f * rollKippRichtung;

            const auto& waende = getNodesInGroup("hauswand");
            for (auto* wand : waende) {
                glm::mat4 invModel = glm::inverse(wand->getGlobalModelMatrix());
                glm::vec3 localPt = glm::vec3(invModel * glm::vec4(transform.position, 1.0f));
                if (localPt.x > wand->localAABB.min.x && localPt.x < wand->localAABB.max.x
                    && localPt.y > wand->localAABB.min.y && localPt.y < wand->localAABB.max.y
                    && localPt.z > wand->localAABB.min.z && localPt.z < wand->localAABB.max.z) {
                    transform.position = oldPos;
                    break;
                }
            }
        }
    } else {
        processInput(deltaTime);
    }
    if (isActive && !istRollend) handleRotations(window, deltaTime);
    if (isActive) updateCameraTransform(cameraTransform, deltaTime);
    if (isActive) handleItemAction(window);
}

void Player::processInput(const float deltaTime) {
    if (Input::isKeyJustPressed(GLFW_KEY_F3) && !getNodesInGroup("player").empty()) {
        setActive(!isActive);
    }

    if (!isActive) return;

    float islandHeight = (benutzeInsel && island != nullptr)
        ? island->getHeight(transform.position.x, transform.position.z)
        : bodenHoehe;

    const auto time = static_cast<float>(glfwGetTime());
    float waterHeight = Uboot::getWaterHeight(transform.position.x, transform.position.z, time) * 2.0f;

    constexpr float standEyeHeight = 3.0f;
    constexpr float swimEyeHeight = 1.1f;

    float standingY = islandHeight + standEyeHeight;
    float floatingY = waterHeight + swimEyeHeight;

    bool isSwimming = (affectedByWater) ? floatingY > standingY : false;

    float baseMoveSpeed = (isSwimming ? 2.5f : 6.0f) * speedMultiplier;
    float activeJumpForce = (isSwimming ? 4.0f : 10.0f) * jumpMultiplier;
    float gravity = 30.0f;

    constexpr float sprintMultiplier = 2.0f;
    bool sprinting = Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT);
    float currentSpeed = sprinting ? baseMoveSpeed * sprintMultiplier : baseMoveSpeed;

    glm::vec3 forward = transform.forward();
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    verticalVelocity -= gravity * deltaTime;
    direction = glm::vec3(0.0f);

    if (Input::isKeyPressed(GLFW_KEY_W)) direction += forward;
    if (Input::isKeyPressed(GLFW_KEY_S)) direction -= forward;
    if (Input::isKeyPressed(GLFW_KEY_A)) direction -= transform.right();
    if (Input::isKeyPressed(GLFW_KEY_D)) direction += transform.right();

    if (Input::isKeyJustPressed(GLFW_KEY_SPACE) && grounded) {
        verticalVelocity = activeJumpForce;
        grounded = false;
    }

    if (!istRollend && !rollAbklingzeit && grounded && !benutzeInsel) {
        glm::vec3 rechts = transform.right();
        rechts.y = 0.0f;
        if (glm::length2(rechts) > 0.001f) rechts = glm::normalize(rechts);
        bool sprint = Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT);
        if (Input::isKeyJustPressed(GLFW_KEY_Z)) {
            istRollend = true;
            rollZeit = 0.0f;
            rollRichtung = -rechts;
            rollSprintMult = sprint ? 2.0f : 1.0f;
            rollKippRichtung = 1.0f;
            verticalVelocity = 0.0f;
            spawnSchutz = rollDauer;
        }
        if (Input::isKeyJustPressed(GLFW_KEY_X)) {
            istRollend = true;
            rollZeit = 0.0f;
            rollRichtung = rechts;
            rollSprintMult = sprint ? 2.0f : 1.0f;
            rollKippRichtung = -1.0f;
            verticalVelocity = 0.0f;
            spawnSchutz = rollDauer;
        }
    }

    if (glm::length2(direction) > 0.0f) {
        direction = glm::normalize(direction);
    }

    glm::vec3 oldPos = transform.position;
    transform.position += direction * currentSpeed * deltaTime;

    const auto& waende = getNodesInGroup("hauswand");
    for (auto* wand : waende) {
        glm::mat4 invModel = glm::inverse(wand->getGlobalModelMatrix());
        glm::vec3 localPt = glm::vec3(invModel * glm::vec4(transform.position, 1.0f));
        if (localPt.x > wand->localAABB.min.x && localPt.x < wand->localAABB.max.x
            && localPt.y > wand->localAABB.min.y && localPt.y < wand->localAABB.max.y
            && localPt.z > wand->localAABB.min.z && localPt.z < wand->localAABB.max.z) {
            transform.position = oldPos;
            break;
        }
    }
    if (affectedByWater) {
        if (isSwimming) {
            float swayX = glm::sin(time * 1.5f) * 0.25f;
            float swayZ = glm::cos(time * 1.2f) * 0.25f;
            transform.position += glm::vec3(swayX, 0.0f, swayZ) * deltaTime;
        }

        float finalTargetY = glm::max(standingY, floatingY);
        if (finalTargetY > targetY) {
            constexpr float upAcceleration = 12.0f;
            const float tUp = 1.0f - glm::exp(-upAcceleration * deltaTime);
            targetY = glm::mix(targetY, finalTargetY, tUp);
        } else {
            constexpr float yAcceleration = 2.0f;
            const float tSpeed = 1.0f - glm::exp(-yAcceleration * deltaTime);
            targetY = glm::mix(targetY, finalTargetY, tSpeed);
        }
    }
    transform.position.y += verticalVelocity * deltaTime;

    if (transform.position.y <= targetY) {
        transform.position.y = targetY;
        verticalVelocity = 0.0f;
        grounded = true;
    }
}

void Player::setActive(const bool active) {
    if (active && !isActive) {
        firstMouse = true;
    }
    isActive = active;
}

void Player::handleRotations(GLFWwindow* window, float deltaTime) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse) {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    const double xoffset = mouseX - lastX;
    const double yoffset = lastY - mouseY;

    lastX = mouseX;
    lastY = mouseY;

    constexpr float mouseSensitivity = 0.15f;
    yaw += glm::radians(static_cast<float>(-xoffset) * mouseSensitivity);
    pitch += glm::radians(static_cast<float>(yoffset) * mouseSensitivity);
    pitch = glm::clamp(pitch, glm::radians(-89.0f), glm::radians(89.0f));

    const glm::quat yawRot = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::quat pitchRot = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));

    transform.rotation = glm::normalize(yawRot * pitchRot);
}

void Player::updateCameraTransform(Transform& cameraTransform, float deltaTime) const {
    cameraTransform.position = transform.position;
    cameraTransform.rotation = transform.rotation;
    if (istRollend) {
        glm::vec3 blickRichtung = glm::normalize(cameraTransform.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
        glm::quat rollTilt = glm::angleAxis(glm::radians(rollWinkel), blickRichtung);
        cameraTransform.rotation = rollTilt * cameraTransform.rotation;
    }
}

void Player::updateRaycast() {
    if (raycast != nullptr && raycast->isColliding()) {
        const glm::vec3 hitPoint = raycast->getCollisionPoint();
        rayTargetHelper->transform.position = hitPoint;
        rayTargetHelper->visible = false;
    } else {
        rayTargetHelper->visible = false;
    }

    raycastTimer->startTimer(0.1f, [this]() { updateRaycast(); });
}

void Player::handleItemAction(GLFWwindow* window) {
    GegenstandID aktiv = Inventar::getInstance().getAktivesItem();

    if (aktiv != GegenstandID::KEINE && parent != nullptr) {
        bool leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        if (aktiv == GegenstandID::TODESSTERN && leftClick && onTodessternBenutzt) {
            int idx = Inventar::getInstance().getAktiverSlot();
            Inventar::getInstance().hotbarVerbrauchen(idx);
            onTodessternBenutzt();
            return;
        }

        const auto& waffenInfo = GegenstandDaten::getInstance().getInfo(aktiv);
        if (waffenInfo.istWaffe && leftClick && kannSchiessen) {
            kannSchiessen = false;
            schussTimer->startTimer(0.15f, [this]() { kannSchiessen = true; });

            glm::vec3 schussPos = transform.position + transform.forward() * 2.0f;
            auto* laser = new SpielerLaser();
            parent->addChild(laser);
            laser->abfeuern(schussPos, transform.rotation);
            AudioManager::getInstance().play2D("assets/audio/shoot.mp3", false, true);
        }
    }

    bool rightClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (!rightClick) {
        kannPlatzieren = true;
        return;
    }
    if (!kannPlatzieren) return;
    kannPlatzieren = false;

    if (aktiv == GegenstandID::KEINE) return;
    if (parent == nullptr) return;

    const auto& info = GegenstandDaten::getInstance().getInfo(aktiv);

    glm::vec3 platzPos = transform.position + transform.forward() * 3.0f;
    if (aktiv == GegenstandID::GLAS) {
        const auto& haeuser = getNodesInGroup("haus");
        float naheste = 15.0f;
        Wesen* nahestesHaus = nullptr;
        for (auto* h : haeuser) {
            float dist = glm::distance(transform.position, h->getGlobalTransform().position);
            if (dist < naheste) {
                naheste = dist;
                nahestesHaus = h;
            }
        }
        if (nahestesHaus) {
            glm::vec3 hausPos = nahestesHaus->getGlobalTransform().position;
            platzPos = hausPos + glm::vec3(0.0f, 6.3f, 3.8f);
        }
    } else if (benutzeInsel && island != nullptr) {
        platzPos.y = island->getHeight(platzPos.x, platzPos.z) + 0.5f;
    } else {
        platzPos.y = bodenHoehe + 0.5f;
    }

    if (info.istWaffe) return;

    if (info.istKonsumierbar) {
        int hotbarIdx = Inventar::getInstance().getAktiverSlot();
        Inventar::getInstance().hotbarVerbrauchen(hotbarIdx);

        speedMultiplier = 2.0f;
        jumpMultiplier = 2.0f;
        buffTimer = 30.0f;

        AudioManager::getInstance().play2D("assets/audio/pickup.mp3", false, true);
        return;
    }

    if (info.istPlatzierbar) {
        int hotbarIdx = Inventar::getInstance().getAktiverSlot();
        Inventar::getInstance().hotbarVerbrauchen(hotbarIdx);

        auto* objekt = new PlatzierbaresObjekt(aktiv);
        objekt->transform.position = platzPos;
        parent->addChild(objekt);

        AudioManager::getInstance().play2D("assets/audio/pickup.mp3", false, true);
    }
}

void Player::schadenNehmen(float schaden) {
    if (spawnSchutz > 0.0f) return;
    kamera.addShake(0.3f, 0.2f);
    schadenBlitz = 1.0f;
    leben -= schaden;
    if (leben <= 0.0f) {
        leben = 100.0f;
        transform.position = spawnPosition;
        targetY = spawnPosition.y;
        spawnSchutz = 2.0f;
    }
}
