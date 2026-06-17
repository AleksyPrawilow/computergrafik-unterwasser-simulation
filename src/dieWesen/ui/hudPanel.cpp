//
// Created by Alexey Pravilov on 10/06/2026.
//

#include "hudPanel.h"

#include "werkzeuge/textur.h"
#include "werkzeuge/ui/uiLabel.h"

void HudPanel::onInit() {
	uboot = dynamic_cast<Uboot* >(getNodesInGroup("player")[0]);

    const GLuint heartTex = Kern::LoadTexture("assets/textures/heart.png", true);
    const GLuint waveTex = Kern::LoadTexture("assets/textures/heart.png", true);
    const GLuint propTex = Kern::LoadTexture("assets/textures/heart.png", true);

	spacing = 12.0f;
    transform.position = glm::vec3(80.0f, 20.0f, 0.0f);

    const auto healthRow = new HBoxUI();
    healthRow->init();
    addChild(healthRow);

    const auto hpIcon = new UIElement();
    hpIcon->init();
    hpIcon->material.albedo = heartTex;
    hpIcon->transform.scale = glm::vec3(72.0f, 72.0f, 1.0f);
    healthRow->addChild(hpIcon);

    hpLabel = new UILabel();
	hpLabel->setText("100", 64.0f);
    hpLabel->init();
    healthRow->addChild(hpLabel);

    const auto depthRow = new HBoxUI();
    depthRow->init();
    addChild(depthRow);

    const auto depthIcon = new UIElement();
    depthIcon->init();
    depthIcon->material.albedo = waveTex;
    depthIcon->transform.scale = glm::vec3(72.0f, 72.0f, 1.0f);
    depthRow->addChild(depthIcon);

    depthLabel = new UILabel();
	depthLabel->setText("DEPTH: 0M", 64.0f);
    depthLabel->init();
    depthRow->addChild(depthLabel);


    const auto speedRow = new HBoxUI();
    speedRow->init();
    addChild(speedRow);

    const auto speedIcon = new UIElement();
    speedIcon->init();
    speedIcon->material.albedo = propTex;
    speedIcon->transform.scale = glm::vec3(72.0f, 72.0f, 1.0f);
    speedRow->addChild(speedIcon);

    speedLabel = new UILabel();
	speedLabel->setText("SPEED: 0M", 64.0f);
    speedLabel->init();
    speedRow->addChild(speedLabel);
}

void HudPanel::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
	VBoxUI::onUpdate(window, deltaTime, cameraTransform);

	if (uboot != nullptr) {
		float depthVal = -uboot->transform.position.y;
		const float speedVal = uboot->getSpeed() * 3.6f;

		if (depthVal < 0.0f) depthVal = 0.0f;

		setDepth(depthVal);
		setSpeed(speedVal);
	}
}

void HudPanel::setHealth(const float newHealth) const {
	hpLabel->text = std::to_string(newHealth);
}

void HudPanel::setDepth(const float newDepth) const {
	depthLabel->text = std::to_string(static_cast<int>(newDepth)) + "m";
}

void HudPanel::setSpeed(const float newSpeed) const {
	speedLabel->text = std::to_string(static_cast<int>(newSpeed)) + "km/h";
}
