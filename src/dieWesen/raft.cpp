//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "raft.h"
#include "player.h"
#include "uboot.h"
#include "werkzeuge/kamera.h"
#include "ui/fadenkreuz.h"
#include "werkzeuge/input.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/modelManager.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/visual/tween.h"
#include "werkzeuge/visual/questManager.h"

void Raft::init() {
    loadModel("assets/models/raft.obj");
    material.albedo = Kern::LoadTexture("assets/textures/Raft_baseColor.png");
    material.normal = Kern::LoadTexture("assets/textures/Raft_normal.png");
    material.roughness = Kern::LoadTexture("assets/textures/Raft_metallicRoughness.png");
    material.metallic = Kern::LoadTexture("assets/textures/Raft_metallicRoughness.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.scale = glm::vec3(0.5f);

    isCollidable = true;

    crosshairPivot = new Wesen();
    parent->addChild(crosshairPivot);
    crosshair = new Fadenkreuz();
    crosshair->init(16.0f / 9.0f);
    crosshair->visible = false;
    crosshairPivot->addChild(crosshair);

    interactLabel = new UILabel();
    interactLabel->text = "Press [E] to board";
    interactLabel->fontSize = 48.0f;
    interactLabel->expansion = UIExpansion::CENTER;
    parent->addChild(interactLabel);
    glm::vec2 screenSize = Kern::GetViewportSize();
    interactLabel->transform.position = glm::vec3(screenSize.x / 2.0f, screenSize.y / 2.0f, 0.0f);
    interactLabel->visible = false;

    fishSound = new AudioPlayer("assets/audio/pickup.mp3", false, 5.0f, true, false);
    addChild(fishSound);

    rodVisual = new Wesen();
    rodVisual->material.albedo = Kern::LoadTexture("assets/textures/Raft_baseColor.png");
    rodVisual->material.shader = ShaderManager::getInstance().getShader("default");
    rodVisual->mesh = ModelManager::getInstance().getModel("assets/models/cube.obj").mesh;
    rodVisual->transform.scale = glm::vec3(0.1f, 0.1f, 3.0f);
    rodVisual->transform.position = rodIdlePos;
    rodVisual->visible = false;
    addChild(rodVisual);
}

void Raft::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    crosshair->visible = isActive;

    if (shouldFloat) {
        updateWavePhysics(deltaTime);
    }

    if (!isActive && player != nullptr
        && player->raycast->isColliding() && player->raycast->getCollider() == this) {
        interactLabel->visible = true;
        if (Input::isKeyJustPressed(GLFW_KEY_E)) {
            isActive = true;
            interactLabel->visible = false;
            player->visible = false;
            player->setActive(false);
            firstMouse = true;
        }
    } else if (!isActive) {
        interactLabel->visible = false;
    }

    if (!isActive) {
        if (rodVisual != nullptr) rodVisual->visible = false;
        if (isFishing) cancelFishing();
        return;
    }

    updateRodVisual();
    processInput(window, deltaTime);
    updateCameraTransform(cameraTransform, deltaTime);

    if (crosshairPivot != nullptr) {
        crosshairPivot->transform.position = cameraTransform.position;
        crosshairPivot->transform.rotation = cameraTransform.rotation;
    }
}

void Raft::processInput(GLFWwindow* window, float deltaTime) {
    extern bool inventarOffen;
    float sprintMult = Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT) ? 2.0f : 1.0f;

    if (Input::isKeyJustPressed(GLFW_KEY_G) && !inventarOffen) {
        if (isFishing) cancelFishing();
        isActive = false;
        player->visible = true;
        player->setActive(true);
        player->transform.position = getGlobalTransform().position + glm::vec3(0.0f, 3.0f, 0.0f);
        return;
    }

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse || inventarOffen) {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
        if (inventarOffen) return;
    }

    double xoffset = mouseX - lastX;
    double yoffset = lastY - mouseY;
    lastX = mouseX;
    lastY = mouseY;

    constexpr float mouseSensitivity = 0.003f;
    currentYaw -= static_cast<float>(xoffset) * mouseSensitivity;
    currentPitch += static_cast<float>(yoffset) * mouseSensitivity;
    currentPitch = glm::clamp(currentPitch, glm::radians(-80.0f), glm::radians(60.0f));

    glm::vec3 forward = glm::normalize(glm::vec3(
        -glm::sin(currentYaw), 0.0f, -glm::cos(currentYaw)));

    if (Input::isKeyPressed(GLFW_KEY_W)) {
        transform.position += forward * moveSpeed * sprintMult * deltaTime;
    }
    if (Input::isKeyPressed(GLFW_KEY_S)) {
        transform.position -= forward * moveSpeedBackward * sprintMult * deltaTime;
    }
    if (Input::isKeyPressed(GLFW_KEY_A)) {
        currentYaw += turnSpeed * deltaTime;
    }
    if (Input::isKeyPressed(GLFW_KEY_D)) {
        currentYaw -= turnSpeed * deltaTime;
    }

    GegenstandID aktiv = Inventar::getInstance().getAktivesItem();

    if (aktiv != GegenstandID::ANGEL && isFishing) {
        cancelFishing();
    }

    if (aktiv == GegenstandID::ANGEL && !isFishing
        && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        isFishing = true;
        fish();
    }
}

void Raft::updateRodVisual() {
    if (rodVisual == nullptr) return;
    GegenstandID aktiv = Inventar::getInstance().getAktivesItem();
    rodVisual->visible = (aktiv == GegenstandID::ANGEL);
    if (rodVisual->visible) {
        rodVisual->transform.rotation = glm::quat(glm::radians(rodEuler));
    }
}

void Raft::cancelFishing() {
    isFishing = false;
    rodEuler = rodIdleEuler;
    if (rodVisual != nullptr) {
        rodVisual->transform.position = rodIdlePos;
    }
}

void Raft::fish() {
    extern Kamera kamera;
    kamera.addShake(0.05f, 0.3f);

    glm::vec3 castPos = glm::vec3(0.6f, 5.0f, -4.0f);
    glm::vec3 castEuler = glm::vec3(-20.0f, 0.0f, 0.0f);
    glm::vec3 jerkEuler = glm::vec3(45.0f, 0.0f, 10.0f);
    glm::vec3 reelPos = glm::vec3(0.6f, 6.5f, -1.0f);
    glm::vec3 reelEuler = glm::vec3(50.0f, 0.0f, 5.0f);

    createTween()
        ->tweenProperty(&rodVisual->transform.position, castPos, 0.4f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&rodEuler, castEuler, 0.4f, EaseType::EASE_OUT_BACK)
        ->tweenInterval(1.5f)
        ->tweenCallback([this]() {
            extern Kamera kamera;
            kamera.addShake(0.03f, 0.3f);
        })
        ->tweenInterval(1.5f)
        ->tweenCallback([this]() {
            extern Kamera kamera;
            kamera.addShake(0.1f, 0.3f);
        })
        ->tweenProperty(&rodEuler, jerkEuler, 0.2f, EaseType::EASE_OUT_CUBIC)
        ->tweenProperty(&rodEuler, glm::vec3(-10.0f, 0.0f, 0.0f), 0.15f, EaseType::EASE_OUT_SINE)
        ->tweenCallback([this]() {
            extern Kamera kamera;
            kamera.addShake(0.2f, 0.4f);
        })
        ->tweenProperty(&rodEuler, jerkEuler, 0.15f, EaseType::EASE_OUT_CUBIC)
        ->tweenInterval(0.3f)
        ->tweenProperty(&rodVisual->transform.position, reelPos, 0.6f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&rodEuler, reelEuler, 0.6f, EaseType::EASE_OUT_BACK)
        ->tweenInterval(1.0f)
        ->tweenProperty(&rodVisual->transform.position, rodIdlePos, 0.5f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&rodEuler, rodIdleEuler, 0.5f, EaseType::EASE_OUT_SINE)
        ->tweenCallback([this]() {
            if (!isFishing) return;
            bool catchFish = (rand() % 100) < 70;
            if (catchFish) {
                Inventar::getInstance().hinzufuegen(GegenstandID::FISCH, 1);
            } else {
                Inventar::getInstance().hinzufuegen(GegenstandID::SEETANG, 1);
            }
            fishSound->play();
            QuestManager::getInstance().progressObjective("catch_fish");
            isFishing = false;
        });
}

void Raft::updateWavePhysics(float deltaTime) {
    const auto time = static_cast<float>(glfwGetTime());
    glm::vec3 pos = transform.position;

    auto rot = glm::mat3(glm::angleAxis(currentYaw, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 corners[4] = {
        {-halfSize.x, 0, -halfSize.y},
        { halfSize.x, 0, -halfSize.y},
        { halfSize.x, 0,  halfSize.y},
        {-halfSize.x, 0,  halfSize.y}
    };

    float heights[4];
    glm::vec3 points[4];

    for (int i = 0; i < 4; i++) {
        points[i] = pos + rot * corners[i];
        heights[i] = Uboot::getWaterHeight(points[i].x, points[i].z, time);
    }

    const float avgHeight = (heights[0] + heights[1] + heights[2] + heights[3]) * 0.25f;

    glm::vec3 targetPos = pos;
    targetPos.y = avgHeight;

    transform.position = glm::mix(
        transform.position,
        targetPos,
        1.0f - exp(-heightSmooth * deltaTime)
    );

    float front = (heights[2] + heights[3]) * 0.5f;
    float back  = (heights[0] + heights[1]) * 0.5f;
    float rightH = (heights[1] + heights[2]) * 0.5f;
    float leftH  = (heights[0] + heights[3]) * 0.5f;

    float pitch = (back - front) * tiltStrength;
    float roll  = (leftH - rightH) * tiltStrength;

    glm::quat yawQuat = glm::angleAxis(currentYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat tiltQuat = glm::quat(glm::vec3(pitch, 0.0f, roll));
    glm::quat targetRot = yawQuat * tiltQuat;

    transform.rotation = glm::slerp(
        transform.rotation,
        targetRot,
        1.0f - glm::exp(-rotSmooth * deltaTime)
    );
}

void Raft::updateCameraTransform(Transform& cameraTransform, float deltaTime) const {
    glm::vec3 pos = transform.position;
    glm::vec3 forward = glm::normalize(glm::vec3(
        -glm::sin(currentYaw), 0.0f, -glm::cos(currentYaw)));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 lookDir = forward * glm::cos(currentPitch) + up * glm::sin(currentPitch);
    glm::vec3 targetCamPos = pos + up * 3.0f;
    glm::vec3 lookTarget = targetCamPos + lookDir * 10.0f;

    const glm::quat currentCamRot = cameraTransform.rotation;
    cameraTransform.lookAt(lookTarget, up);
    const glm::quat targetCamRot = cameraTransform.rotation;

    float tFollow = 1.0f - glm::exp(-6.0f * deltaTime);
    float tRotate = 1.0f - glm::exp(-8.0f * deltaTime);

    cameraTransform.position = glm::mix(cameraTransform.position, targetCamPos, tFollow);
    cameraTransform.rotation = glm::slerp(currentCamRot, targetCamRot, tRotate);
}
