#include "ausruestungsLeiste.h"
#include "werkzeuge/input.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/renderWerkzeuge.h"

extern bool inventarOffen;

void AusruestungsLeiste::onInit() {
    leisteContainer = new HBoxUI(6.0f);
    leisteContainer->setAlignment(UIAlignment::CENTER);
    addChild(leisteContainer);

    for (int i = 0; i < Inventar::HOTBAR_GROESSE; i++) {
        auto* spalte = new VBoxUI(2.0f);
        spalte->setAlignment(UIAlignment::CENTER);
        leisteContainer->addChild(spalte);

        auto* slot = new InventarSlotUI();
        spalte->addChild(slot);
        slots[i] = slot;

        auto* label = new UILabel();
        label->setText(std::to_string(i + 1), 12.0f);
        label->color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
        spalte->addChild(label);
        nummerLabels[i] = label;
    }

    Inventar::getInstance().addOnChanged([this]() { aktualisieren(); });
    aktualisieren();
}

void AusruestungsLeiste::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    glm::vec2 viewport = Kern::GetViewportSize();
    float barW = leisteContainer->transform.scale.x;
    transform.position = glm::vec3((viewport.x - barW) * 0.5f, viewport.y - 90.0f, 0.0f);

    if (!inventarOffen) {
        constexpr int keys[] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5 };
        for (int i = 0; i < 5; i++) {
            if (Input::isKeyJustPressed(keys[i])) {
                Inventar::getInstance().setAktiverSlot(i);
            }
        }
    }

    aktualisieren();
}

void AusruestungsLeiste::aktualisieren() {
    int aktiv = Inventar::getInstance().getAktiverSlot();
    for (int i = 0; i < Inventar::HOTBAR_GROESSE; i++) {
        const auto& platz = Inventar::getInstance().getHotbar(i);
        if (platz.istLeer()) {
            slots[i]->leeren();
        } else {
            slots[i]->setInhalt(platz.id, platz.anzahl);
        }
        slots[i]->setAusgewaehlt(i == aktiv);
        nummerLabels[i]->color = (i == aktiv)
            ? glm::vec4(1.0f, 0.85f, 0.3f, 1.0f)
            : glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    }
}
