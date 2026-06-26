//
// Created by Alexey Pravilov on 26/06/2026.
//

#include "spaceshipEnterable.h"
#include "ui/fadeOverlay.h"

#include "werkzeuge/input.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void SpaceshipEnterable::init() {
    isCollidable = true;
    loadModel("assets/models/spaceship.obj");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.albedo = Kern::LoadTexture("assets/textures/spaceship.png");
    material.metallic = Kern::LoadTexture("assets/textures/spaceship_metalness.png");
    material.normal = Kern::LoadTexture("assets/textures/spaceship_normal.png");
    material.emission = Kern::LoadTexture("assets/textures/spaceship_emissive.png");

    player = dynamic_cast<Player * >(getNodesInGroup("playerWalking")[0]);

    interactLabel = new UILabel();
    interactLabel->text = "Press [E] to pilot";
    interactLabel->fontSize = 48.0f;
    interactLabel->expansion = UIExpansion::CENTER;
    parent->addChild(interactLabel);
    glm::vec2 screenSize = Kern::GetViewportSize();
    interactLabel->transform.position = glm::vec3(screenSize.x / 2.0f, screenSize.y / 2.0f, 0.0f);
    interactLabel->visible = false;
}

void SpaceshipEnterable::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    transform.rotation = glm::quat(glm::radians(euler));

    if (cutsceneTriggered) {
        float camFollowSpeed = 6.0f;
        float camRotateSpeed = 3.0f;
        glm::vec3 targetPos = transform.position;
        const glm::quat currentCamRot = cameraTransform.rotation;
        cameraTransform.lookAt(targetPos, glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::quat targetCamRot = cameraTransform.rotation;
        const float tRotate = 1.0f - glm::exp(-camRotateSpeed * deltaTime);
        cameraTransform.rotation = glm::slerp(currentCamRot, targetCamRot, tRotate);
        return;
    }

    if (player->raycast->isColliding() && player->raycast->getCollider() == this) {
        interactLabel->visible = true;
        if (Input::isKeyJustPressed(GLFW_KEY_E)) {
            player->setActive(false);
            interactLabel->visible = false;
            cutscene();
        }
    } else {
        interactLabel->visible = false;
    }
}

void SpaceshipEnterable::cutscene() {
    auto * cameraPos = getNodesInGroup("CameraPos")[0];
    cutsceneTriggered = true;
    createTween()
        ->tweenProperty(&kamera.transform.position, cameraPos->transform.position, 1.0f, EaseType::EASE_OUT_CIRC)
        ->tweenInterval(1.0)
        ->tweenProperty(&transform.position.y, transform.position.y + 5.0f, 4.0f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&euler.y, -10.0f, 6.0f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&euler.z, -70.0f, 6.0f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&transform.position.y, transform.position.y + 1000.0f, 4.0f, EaseType::EASE_IN)
        ->parallel()
        ->tweenProperty(&transform.position.x, transform.position.x - 200.0f, 4.0f, EaseType::EASE_IN)
        ->parallel()
        ->tweenProperty(&transform.position.z, transform.position.z - 100.0f, 4.0f, EaseType::EASE_IN)
        ->parallel()
        ->tweenProperty(&euler.x, -360.0f * 4, 3.0f, EaseType::EASE_IN)
        ->parallel()
        ->tweenCallback([this]() {
        auto* fade = new FadeOverlay();
        parent->addChild(fade);
        fade->fadeIn(2.0f);
        });
}