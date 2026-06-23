#include "inventarSlotUI.h"
#include "werkzeuge/textur.h"

GLuint InventarSlotUI::slotTextur = 0;
GLuint InventarSlotUI::slotAusgewaehltTextur = 0;
bool InventarSlotUI::texturenGeladen = false;

void InventarSlotUI::ladeTexturen() {
    if (texturenGeladen) return;
    slotTextur = Kern::LoadTexture("assets/textures/inventar_slot.png");
    slotAusgewaehltTextur = Kern::LoadTexture("assets/textures/inventar_slot_selected.png");
    texturenGeladen = true;
}

void InventarSlotUI::onInit() {
    ladeTexturen();

    transform.scale = glm::vec3(64.0f, 64.0f, 1.0f);

    hintergrund = new UIElement();
    addChild(hintergrund);
    hintergrund->material.albedo = slotTextur;
    hintergrund->transform.scale = glm::vec3(64.0f, 64.0f, 1.0f);

    icon = new UIElement();
    addChild(icon);
    icon->transform.position = glm::vec3(4.0f, 4.0f, 0.0f);
    icon->transform.scale = glm::vec3(56.0f, 56.0f, 1.0f);
    icon->material.albedo = 0;

    anzahlLabel = new UILabel();
    addChild(anzahlLabel);
    anzahlLabel->setText("", 18.0f);
    anzahlLabel->transform.position = glm::vec3(38.0f, 42.0f, 0.0f);
    anzahlLabel->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void InventarSlotUI::setInhalt(GegenstandID id, int anzahl) {
    if (id == GegenstandID::KEINE || anzahl <= 0) {
        leeren();
        return;
    }

    const auto& info = GegenstandDaten::getInstance().getInfo(id);
    icon->material.albedo = info.iconTextur;

    if (anzahl > 1) {
        anzahlLabel->setText(std::to_string(anzahl), 18.0f);
    } else {
        anzahlLabel->setText("", 14.0f);
    }
}

void InventarSlotUI::leeren() {
    icon->material.albedo = 0;
    anzahlLabel->setText("", 14.0f);
}

void InventarSlotUI::setAusgewaehlt(bool ausgewaehlt) {
    istAusgewaehlt = ausgewaehlt;
    hintergrund->material.albedo = ausgewaehlt ? slotAusgewaehltTextur : slotTextur;
}
