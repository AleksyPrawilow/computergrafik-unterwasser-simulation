#include "weltraumHudPanel.h"

#include "werkzeuge/textur.h"

void WeltraumHudPanel::onInit() {
    const auto& spieler = getNodesInGroup("spieler");
    if (!spieler.empty()) {
        raumschiff = dynamic_cast<Raumschiff*>(spieler[0]);
    }

    const GLuint herzTextur = Kern::LoadTexture("assets/textures/heart.png", true);

    spacing = 12.0f;
    transform.position = glm::vec3(80.0f, 20.0f, 0.0f);

    const auto lebenReihe = new HBoxUI();
    lebenReihe->init();
    addChild(lebenReihe);

    const auto hpIcon = new UIElement();
    hpIcon->init();
    hpIcon->material.albedo = herzTextur;
    hpIcon->transform.scale = glm::vec3(72.0f, 72.0f, 1.0f);
    lebenReihe->addChild(hpIcon);

    hpLabel = new UILabel();
    hpLabel->setText("100", 64.0f);
    hpLabel->init();
    lebenReihe->addChild(hpLabel);

    const auto tempoReihe = new HBoxUI();
    tempoReihe->init();
    addChild(tempoReihe);

    const auto tempoIcon = new UIElement();
    tempoIcon->init();
    tempoIcon->material.albedo = herzTextur;
    tempoIcon->transform.scale = glm::vec3(72.0f, 72.0f, 1.0f);
    tempoReihe->addChild(tempoIcon);

    geschwindigkeitLabel = new UILabel();
    geschwindigkeitLabel->setText("0km/h", 64.0f);
    geschwindigkeitLabel->init();
    tempoReihe->addChild(geschwindigkeitLabel);
}

void WeltraumHudPanel::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    VBoxUI::onUpdate(window, deltaTime, cameraTransform);

    if (raumschiff != nullptr) {
        hpLabel->text = std::to_string(static_cast<int>(raumschiff->getLeben()));
        const float tempo = raumschiff->getGeschwindigkeit() * 3.6f;
        geschwindigkeitLabel->text = std::to_string(static_cast<int>(tempo)) + "km/h";
    }
}
