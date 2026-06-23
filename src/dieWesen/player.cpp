//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "player.h"
#include "axe.h"
#include "aufhebbar.h"
#include "grabLoch.h"
#include "platzierbaresObjekt.h"
#include "ui/fadenkreuz.h"
#include "werkzeuge/input.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/audioManager.h"

void Player::init() {
    transform.scale = glm::vec3(1.0f);
    transform.position = glm::vec3(-700.0f, 0.0f, -220.0f);
    addToGroup("spielerInsel");
    island = dynamic_cast<Island *>(getNodesInGroup("Island")[0]);
    auto * crosshair = new Fadenkreuz();
    crosshair->init(16.0f / 7.0f);
    addChild(crosshair);

    auto * axe = new Axe();
    addChild(axe);

    rayTargetHelper = new Wesen();
    rayTargetHelper->loadModel("assets/models/hitPoint.obj");
    rayTargetHelper->material.shader = ShaderManager::getInstance().getShader("default");
    rayTargetHelper->material.opacity = Kern::LoadTexture("assets/textures/jellyfish_opacity.png");
    rayTargetHelper->material.albedo = Kern::LoadTexture("assets/textures/jellyfish_inner_albedo.png");
    rayTargetHelper->material.isTransparent = true;
    rayTargetHelper->transform.scale = glm::vec3(0.25f);
    parent->addChild(rayTargetHelper);

    raycast = new RayCast();
    raycast->targetPosition = glm::vec3(0.0f, 0.0f, -2.5f);
    addChild(raycast);

    raycastTimer = new Timer();
    addChild(raycastTimer);
    raycastTimer->startTimer(0.1f, [this]() { updateRaycast(); });
}

void Player::onUpdate(GLFWwindow* window, const float deltaTime, Transform& cameraTransform) {
    processInput(deltaTime);
    if (isActive) handleRotations(window, deltaTime);
    if (isActive) updateCameraTransform(cameraTransform, deltaTime);
    if (isActive) handleItemAction(window);
}

void Player::processInput(const float deltaTime) {
    if (Input::isKeyJustPressed(GLFW_KEY_F3)) {
        setActive(!isActive);
    }

    if (!isActive) return;

    constexpr float moveSpeed = 6.0f;
    constexpr float gravity = 30.0f;
    constexpr float jumpForce = 10.0f;

    glm::vec3 forward = transform.forward();
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    verticalVelocity -= gravity * deltaTime;
    direction = glm::vec3(0.0f);

    if (Input::isKeyPressed(GLFW_KEY_W)) {
        direction += forward;
    }
    if (Input::isKeyPressed(GLFW_KEY_S)) {
        direction -= forward;
    }
    if (Input::isKeyPressed(GLFW_KEY_A)) {
        direction -= transform.right();
    }
    if (Input::isKeyPressed(GLFW_KEY_D)) {
        direction += transform.right();
    }
    if (Input::isKeyJustPressed(GLFW_KEY_SPACE) && grounded) {
        verticalVelocity = jumpForce;
        grounded = false;
    }

    if (glm::length2(direction) > 0.0f) {
        direction = glm::normalize(direction);
    }

    transform.position += direction * moveSpeed * deltaTime;

    constexpr float yAcceleration = 2.0f;
    const float tSpeed = 1.0f - glm::exp(-yAcceleration * deltaTime);

    targetY = glm::mix(targetY, island->getHeight(transform.position.x, transform.position.z) + 3.0f, tSpeed);
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
        rayTargetHelper->visible = true;
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

    glm::vec3 platzPos = transform.position + transform.forward() * 3.0f;
    platzPos.y = island->getHeight(platzPos.x, platzPos.z) + 0.5f;

    if (aktiv == GegenstandID::SCHAUFEL) {
        if (island != nullptr) {
            island->graben(platzPos, 2.0f, 1.5f);
        }

        auto* erde = new Aufhebbar(GegenstandID::ERDE, 1);
        erde->transform.position = platzPos;
        erde->transform.position.y += 0.5f;
        parent->addChild(erde);

        AudioManager::getInstance().play2D("assets/audio/pickup.mp3", false, true);
        return;
    }

    bool istPlatzierbar = (aktiv == GegenstandID::WERKBANK ||
                           aktiv == GegenstandID::ZAUN ||
                           aktiv == GegenstandID::TRUHE ||
                           aktiv == GegenstandID::FACKEL);

    if (istPlatzierbar) {
        int hotbarIdx = Inventar::getInstance().getAktiverSlot();
        Inventar::getInstance().hotbarEntfernen(hotbarIdx);

        auto* objekt = new PlatzierbaresObjekt(aktiv);
        objekt->transform.position = platzPos;
        parent->addChild(objekt);

        AudioManager::getInstance().play2D("assets/audio/pickup.mp3", false, true);
    }
}
