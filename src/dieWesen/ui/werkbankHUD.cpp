#include "werkbankHUD.h"
#include "dieWesen/player.h"
#include "werkzeuge/input.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/renderWerkzeuge.h"
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/visual/questManager.h"
#include <iostream>

extern bool cursorDisabled;
extern bool inventarOffen;

static void setzeSichtbarkeitRekursiv(Wesen* wesen, bool sichtbar) {
    wesen->visible = sichtbar;
    for (auto* kind : wesen->children) {
        setzeSichtbarkeitRekursiv(kind, sichtbar);
    }
}

void WerkbankHUD::onInit() {
    hintergrund = new UIElement();
    addChild(hintergrund);
    hintergrund->material.albedo = Kern::LoadTexture("assets/textures/inventar_hintergrund.png");

    container = new VBoxUI(6.0f);
    container->setAlignment(UIAlignment::CENTER);
    addChild(container);

    titelLabel = new UILabel();
    titelLabel->setText("Workbench", 36.0f);
    titelLabel->color = glm::vec4(1.0f, 0.8f, 0.4f, 1.0f);
    container->addChild(titelLabel);

    rezepteRegistrieren();

    for (auto& rezept : rezepte) {
        auto* zeile = new RezeptZeileUI();
        zeile->rezept = rezept;
        container->addChild(zeile);
        zeilen.push_back(zeile);
    }

    hinweisLabel = new UILabel();
    hinweisLabel->setText("[Arrows] Select [E] Craft", 14.0f);
    hinweisLabel->color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    container->addChild(hinweisLabel);

    if (!zeilen.empty()) {
        zeilen[0]->setHervorgehoben(true);
    }

    Inventar::getInstance().addOnChanged([this]() { aktualisieren(); });

    setzeSichtbarkeitRekursiv(this, false);
    offen = false;
}

void WerkbankHUD::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (Input::isKeyJustPressed(GLFW_KEY_Q)) {
        if (offen) {
            umschalten(window);
        } else if (!inventarOffen && spielerNaheWerkbank()) {
            umschalten(window);
        }
    }

    if (!offen) return;

    glm::vec2 viewport = Kern::GetViewportSize();
    int sichtbar = glm::min(MAX_SICHTBAR, static_cast<int>(zeilen.size()));
    float zeilenH = sichtbar > 0 ? (zeilen[0]->transform.scale.y + container->spacing) * sichtbar : 0.0f;
    float contentH = titelLabel->transform.scale.y + container->spacing + zeilenH + container->spacing + hinweisLabel->transform.scale.y;
    float contentW = container->transform.scale.x;
    float pad = 20.0f;
    float bgW = glm::max(contentW + pad * 2.0f, 300.0f);
    float bgH = contentH + pad * 3.0f;

    float left = (viewport.x - bgW) * 0.5f;
    float top = (viewport.y - bgH) * 0.5f;

    hintergrund->transform.position = glm::vec3(left, top, 0.0f);
    hintergrund->transform.scale = glm::vec3(bgW, bgH, 1.0f);

    float contentLeft = left + (bgW - contentW) * 0.5f;
    float contentTop = top + pad;
    container->transform.position = glm::vec3(contentLeft, contentTop, 0.0f);

    if (Input::isKeyJustPressed(GLFW_KEY_UP)) {
        if (!zeilen.empty()) {
            zeilen[ausgewaehlteZeile]->setHervorgehoben(false);
            ausgewaehlteZeile = (ausgewaehlteZeile - 1 + static_cast<int>(zeilen.size())) % static_cast<int>(zeilen.size());
            zeilen[ausgewaehlteZeile]->setHervorgehoben(true);
            scrollAktualisieren();
        }
    }

    if (Input::isKeyJustPressed(GLFW_KEY_DOWN)) {
        if (!zeilen.empty()) {
            zeilen[ausgewaehlteZeile]->setHervorgehoben(false);
            ausgewaehlteZeile = (ausgewaehlteZeile + 1) % static_cast<int>(zeilen.size());
            zeilen[ausgewaehlteZeile]->setHervorgehoben(true);
            scrollAktualisieren();
        }
    }

    if (Input::isKeyJustPressed(GLFW_KEY_E)) {
        herstellen();
    }
}

bool WerkbankHUD::spielerNaheWerkbank() const {
    const auto& spieler = getNodesInGroup("spielerInsel");
    const auto& werkbaenke = getNodesInGroup("werkbank");
    if (spieler.empty() || werkbaenke.empty()) return false;

    glm::vec3 spielerPos = spieler[0]->getGlobalTransform().position;
    for (auto* wb : werkbaenke) {
        if (glm::distance(spielerPos, wb->getGlobalTransform().position) < 5.0f) {
            return true;
        }
    }
    return false;
}

void WerkbankHUD::umschalten(GLFWwindow* window) {
    offen = !offen;
    inventarOffen = offen;

    setzeSichtbarkeitRekursiv(this, offen);

    if (offen) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorDisabled = false;
        aktualisieren();
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorDisabled = true;
    }

    const auto& spieler = getNodesInGroup("spielerInsel");
    if (!spieler.empty()) {
        auto* player = dynamic_cast<Player*>(spieler[0]);
        if (player) player->setActive(!offen);
    }
}

void WerkbankHUD::aktualisieren() {
    for (auto* zeile : zeilen) {
        zeile->aktualisieren();
    }
}

void WerkbankHUD::rezepteRegistrieren() {
    rezepte.push_back({GegenstandID::HOLZ, GegenstandID::HOLZ, GegenstandID::PLANKE, 2});
    rezepte.push_back({GegenstandID::HOLZ, GegenstandID::STEIN, GegenstandID::AXT, 1});
    rezepte.push_back({GegenstandID::PLANKE, GegenstandID::STEIN, GegenstandID::SCHAUFEL, 1});
    rezepte.push_back({GegenstandID::PLANKE, GegenstandID::SEIL, GegenstandID::FLOSS, 1});
    rezepte.push_back({GegenstandID::HOLZ, GegenstandID::SEIL, GegenstandID::FACKEL, 2});
    rezepte.push_back({GegenstandID::PLANKE, GegenstandID::PLANKE, GegenstandID::ZAUN, 3});
    rezepte.push_back({GegenstandID::PLANKE, GegenstandID::AXT, GegenstandID::TRUHE, 1});
}

void WerkbankHUD::herstellen() {
    if (zeilen.empty()) return;
    auto* zeile = zeilen[ausgewaehlteZeile];
    if (!zeile->herstellbar) {
        std::cout << "[Workbench] Not enough ingredients!" << std::endl;
        return;
    }

    const auto& r = zeile->rezept;
    auto& inv = Inventar::getInstance();

    if (r.eingabe1 == r.eingabe2) {
        if (!inv.entfernen(r.eingabe1, 2)) return;
    } else {
        if (!inv.entfernen(r.eingabe1, 1)) return;
        if (!inv.entfernen(r.eingabe2, 1)) {
            inv.hinzufuegen(r.eingabe1, 1);
            return;
        }
    }

    inv.hinzufuegen(r.ausgabe, r.ausgabeAnzahl);
    AudioManager::getInstance().play2D("assets/audio/craft.mp3", false, true);

    const auto& info = GegenstandDaten::getInstance().getInfo(r.ausgabe);
    std::cout << "[Workbench] Crafted: " << info.name << " x" << r.ausgabeAnzahl << std::endl;
    QuestManager::getInstance().progressObjective("craft_" + info.name, 1);
}

void WerkbankHUD::scrollAktualisieren() {
    if (ausgewaehlteZeile < scrollOffset) {
        scrollOffset = ausgewaehlteZeile;
    } else if (ausgewaehlteZeile >= scrollOffset + MAX_SICHTBAR) {
        scrollOffset = ausgewaehlteZeile - MAX_SICHTBAR + 1;
    }

    for (int i = 0; i < static_cast<int>(zeilen.size()); i++) {
        bool sichtbar = (i >= scrollOffset && i < scrollOffset + MAX_SICHTBAR);
        zeilen[i]->visible = sichtbar;
        for (auto* kind : zeilen[i]->children) {
            kind->visible = sichtbar;
        }
    }
}
