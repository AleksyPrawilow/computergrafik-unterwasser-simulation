//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "player.h"
#include "axe.h"
#include "ui/fadenkreuz.h"
#include "werkzeuge/input.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Player::init() {
    transform.scale = glm::vec3(1.0f);
    transform.position = glm::vec3(-700.0f, 0.0f, -220.0f);
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
}

void Player::processInput(const float deltaTime) {
    if (Input::isKeyJustPressed(GLFW_KEY_3)) {
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
