//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "player.h"
#include "uboot.h"
#include "heldItem.h"
#include "aufhebbar.h"
#include "grabLoch.h"
#include "platzierbaresObjekt.h"
#include "ui/fadenkreuz.h"
#include "werkzeuge/input.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/visual/questManager.h"

void Player::init() {
    transform.scale = glm::vec3(1.0f);
    transform.position = glm::vec3(-700.0f, 0.0f, -220.0f);
    addToGroup("spielerInsel");
    island = dynamic_cast<Island *>(getNodesInGroup("Island")[0]);
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

    addToGroup("playerWalking");
}

void Player::onUpdate(GLFWwindow* window, const float deltaTime, Transform& cameraTransform) {
    processInput(deltaTime);
    if (isActive) handleRotations(window, deltaTime);
    if (isActive) updateCameraTransform(cameraTransform, deltaTime);
    if (isActive) handleItemAction(window);
}

void Player::processInput(const float deltaTime) {
    if (Input::isKeyJustPressed(GLFW_KEY_F3) && !getNodesInGroup("player").empty()) {
        setActive(!isActive);
    }

    if (!isActive) return;

    float islandHeight = island->getHeight(transform.position.x, transform.position.z);

    const auto time = static_cast<float>(glfwGetTime());
    float waterHeight = Uboot::getWaterHeight(transform.position.x, transform.position.z, time) * 2.0f;

    constexpr float standEyeHeight = 3.0f;
    constexpr float swimEyeHeight = 1.1f;

    float standingY = islandHeight + standEyeHeight;
    float floatingY = waterHeight + swimEyeHeight;

    bool isSwimming = (affectedByWater) ? floatingY > standingY : false;

    float baseMoveSpeed = isSwimming ? 2.5f : 6.0f;
    float activeJumpForce = isSwimming ? 4.0f : 10.0f;
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
    cameraTransform.rotation = transform.rotation;
    cameraTransform.position = transform.position;
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
    platzPos.y = island->getHeight(platzPos.x, platzPos.z) + 0.5f;

    if (info.istPlatzierbar) {
        int hotbarIdx = Inventar::getInstance().getAktiverSlot();
        Inventar::getInstance().hotbarVerbrauchen(hotbarIdx);

        auto* objekt = new PlatzierbaresObjekt(aktiv);
        objekt->transform.position = platzPos;
        parent->addChild(objekt);

        AudioManager::getInstance().play2D("assets/audio/pickup.mp3", false, true);
    }
}
