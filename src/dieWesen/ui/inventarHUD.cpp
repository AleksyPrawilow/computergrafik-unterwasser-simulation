#include "inventarHUD.h"
#include "dieWesen/player.h"
#include "werkzeuge/input.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/renderWerkzeuge.h"
#include "werkzeuge/textur.h"

extern bool cursorDisabled;
extern bool inventarOffen;

static void setzeSichtbarkeitRekursiv(Wesen* wesen, bool sichtbar) {
    wesen->visible = sichtbar;
    for (auto* kind : wesen->children) {
        setzeSichtbarkeitRekursiv(kind, sichtbar);
    }
}

void InventarHUD::onInit() {
    hintergrund = new UIElement();
    addChild(hintergrund);
    hintergrund->material.albedo = Kern::LoadTexture("assets/textures/inventar_hintergrund.png");

    rasterContainer = new VBoxUI(8.0f);
    rasterContainer->setAlignment(UIAlignment::CENTER);
    addChild(rasterContainer);

    titelLabel = new UILabel();
    titelLabel->setText("Inventory", 36.0f);
    titelLabel->color = glm::vec4(1.0f, 0.95f, 0.8f, 1.0f);
    rasterContainer->addChild(titelLabel);

    for (int zeile = 0; zeile < Inventar::ZEILEN; zeile++) {
        auto* reihe = new HBoxUI(4.0f);
        reihe->setAlignment(UIAlignment::CENTER);
        rasterContainer->addChild(reihe);

        for (int spalte = 0; spalte < Inventar::SPALTEN; spalte++) {
            auto* slot = new InventarSlotUI();
            reihe->addChild(slot);
            slots[zeile * Inventar::SPALTEN + spalte] = slot;
        }
    }

    hinweisLabel = new UILabel();
    hinweisLabel->setText("[Arrows] Select [1-5] Equip [C] Crafting", 14.0f);
    hinweisLabel->color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    rasterContainer->addChild(hinweisLabel);

    Inventar::getInstance().addOnChanged([this]() { aktualisieren(); });

    setzeSichtbarkeitRekursiv(this, false);
    offen = false;
}

void InventarHUD::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (Input::isKeyJustPressed(GLFW_KEY_I)) {
        umschalten(window);
    }

    if (!offen) return;

    glm::vec2 viewport = Kern::GetViewportSize() / UIElement::dpiScale;

    float contentH = rasterContainer->transform.scale.y;
    float contentW = rasterContainer->transform.scale.x;
    float pad = 20.0f;
    float bgW = glm::max(contentW + pad * 2.0f, 260.0f);
    float bgH = contentH + pad * 3.0f;

    float left = (viewport.x - bgW) * 0.5f;
    float top = (viewport.y - bgH) * 0.5f;

    hintergrund->transform.position = glm::vec3(left, top, 0.0f);
    hintergrund->transform.scale = glm::vec3(bgW, bgH, 1.0f);

    float contentLeft = left + (bgW - contentW) * 0.5f;
    float contentTop = top + pad;
    rasterContainer->transform.position = glm::vec3(contentLeft, contentTop, 0.0f);

    navigieren();
    ausruesten();
}

void InventarHUD::navigieren() {
    int zeile = ausgewaehlterIndex / Inventar::SPALTEN;
    int spalte = ausgewaehlterIndex % Inventar::SPALTEN;
    bool changed = false;

    if (Input::isKeyJustPressed(GLFW_KEY_LEFT)) {
        spalte = (spalte - 1 + Inventar::SPALTEN) % Inventar::SPALTEN;
        changed = true;
    }
    if (Input::isKeyJustPressed(GLFW_KEY_RIGHT)) {
        spalte = (spalte + 1) % Inventar::SPALTEN;
        changed = true;
    }
    if (Input::isKeyJustPressed(GLFW_KEY_UP)) {
        zeile = (zeile - 1 + Inventar::ZEILEN) % Inventar::ZEILEN;
        changed = true;
    }
    if (Input::isKeyJustPressed(GLFW_KEY_DOWN)) {
        zeile = (zeile + 1) % Inventar::ZEILEN;
        changed = true;
    }

    if (changed) {
        ausgewaehlterIndex = zeile * Inventar::SPALTEN + spalte;
        auswahlAktualisieren();
    }
}

void InventarHUD::ausruesten() {
    constexpr int keys[] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5 };
    const auto& platz = Inventar::getInstance().getPlatz(ausgewaehlterIndex);

    for (int i = 0; i < 5; i++) {
        if (Input::isKeyJustPressed(keys[i])) {
            if (!platz.istLeer()) {
                Inventar::getInstance().ausruesten(i, platz.id);
            }
        }
    }
}

void InventarHUD::umschalten(GLFWwindow* window) {
    offen = !offen;
    inventarOffen = offen;

    setzeSichtbarkeitRekursiv(this, offen);

    if (offen) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorDisabled = false;
        aktualisieren();
        auswahlAktualisieren();
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorDisabled = true;
    }

    const auto& spieler = getNodesInGroup("spielerInsel");
    if (!spieler.empty()) {
        auto* player = dynamic_cast<Player*>(spieler[0]);
        if (player) {
            if (offen) {
                playerWarAktiv = player->getActive();
                player->setActive(false);
            } else {
                player->setActive(playerWarAktiv);
            }
        }
    }
}

void InventarHUD::aktualisieren() {
    for (int i = 0; i < Inventar::GROESSE; i++) {
        const auto& platz = Inventar::getInstance().getPlatz(i);
        if (platz.istLeer()) {
            slots[i]->leeren();
        } else {
            slots[i]->setInhalt(platz.id, platz.anzahl);
        }
    }
    auswahlAktualisieren();
}

void InventarHUD::auswahlAktualisieren() {
    for (int i = 0; i < Inventar::GROESSE; i++) {
        slots[i]->setAusgewaehlt(i == ausgewaehlterIndex);
    }
}
