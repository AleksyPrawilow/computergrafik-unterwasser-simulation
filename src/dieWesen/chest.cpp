//
// Created by Alexey Pravilov on 24/06/2026.
//

#include "chest.h"

#include "aufhebbar.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/input.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/ui/wesenUI.h"
#include "werkzeuge/visual/questManager.h"
#include "werkzeuge/visual/tween.h"

void Chest::init() {
    loadModel("assets/models/chest_base.obj");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.albedo = Kern::LoadTexture("assets/textures/chest_albedo.png");
    material.normal = Kern::LoadTexture("assets/textures/chest_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/chest_metallic.png");
    material.roughness = material.metallic;
    material.emission = Kern::LoadTexture("assets/textures/chest_emissive.png");

    opening = new Wesen();
    opening->loadModel("assets/models/chest_opening.obj");
    opening->material.shader = ShaderManager::getInstance().getShader("default");
    opening->material.albedo = Kern::LoadTexture("assets/textures/chest_albedo.png");
    opening->material.normal = Kern::LoadTexture("assets/textures/chest_normal.png");
    opening->material.metallic = Kern::LoadTexture("assets/textures/chest_metallic.png");
    opening->material.roughness = material.metallic;
    opening->material.emission = Kern::LoadTexture("assets/textures/chest_emissive.png");
    addChild(opening);

    transform.scale = glm::vec3(0.5f);
    isCollidable = true;
    player = dynamic_cast<Player *>(getNodesInGroup("playerWalking")[0]);

    interactLabel = new UILabel();
    interactLabel->text = "Press [E] to open";
    interactLabel->fontSize = 48.0f;
    interactLabel->expansion = UIExpansion::CENTER;
    parent->addChild(interactLabel);
    glm::vec2 screenSize = Kern::GetViewportSize();
    interactLabel->transform.position = glm::vec3(screenSize.x / 2.0f, screenSize.y / 2.0f, 0.0f);
    interactLabel->visible = false;
}

void Chest::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (isOpened) return;

    if (player->raycast->isColliding() && player->raycast->getCollider() == this) {
        interactLabel->visible = true;
        if (Input::isKeyJustPressed(GLFW_KEY_E)) {
            isOpened = true;
            open();
            interactLabel->visible = false;
        }
    } else {
        interactLabel->visible = false;
    }
}

void Chest::open() {
    auto* miniUboot = new Aufhebbar(GegenstandID::MINIUBOOT, 1);
    miniUboot->transform.position = getGlobalTransform().position + glm::vec3(2.0f, 1.25f, 0.0f);
    parent->addChild(miniUboot);
    isCollidable = false;
    QuestManager::getInstance().progressObjective("dig_out");

    createTween()
        ->tweenProperty(&opening->transform.position.z, 4.2f, 0.5f, EaseType::EASE_OUT_CIRC)
        ->tweenProperty(&opening->transform.position.y, -4.0f, 1.0f, EaseType::EASE_OUT);
}
