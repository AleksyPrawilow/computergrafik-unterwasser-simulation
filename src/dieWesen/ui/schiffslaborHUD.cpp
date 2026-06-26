#include "schiffslaborHUD.h"
#include "dieWesen/player.h"
#include "dieWesen/ui/questCompletedBanner.h"
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

void SchiffslaborHUD::onInit() {
    hintergrund = new UIElement();
    addChild(hintergrund);
    hintergrund->material.albedo = Kern::LoadTexture("assets/textures/inventar_hintergrund.png");

    container = new UIElement();
    addChild(container);

    titelLabel = new UILabel();
    titelLabel->setText("Ship Lab", 36.0f);
    titelLabel->color = glm::vec4(0.3f, 0.9f, 0.9f, 1.0f);
    container->addChild(titelLabel);

    rezepte = GegenstandDaten::getInstance().getRezepteFuer(RezeptQuelle::SCHIFFSLABOR);

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

void SchiffslaborHUD::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (Input::isKeyJustPressed(GLFW_KEY_R)) {
        if (offen) {
            umschalten(window);
        } else if (!inventarOffen && spielerNaheKonsole()) {
            umschalten(window);
        }
    }

    if (!offen) return;

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

    constexpr float spacing = 6.0f;
    constexpr float pad = 20.0f;
    float rowH = zeilen.empty() ? 30.0f : zeilen[0]->transform.scale.y;

    float y = 0.0f;
    titelLabel->transform.position = glm::vec3(0.0f, y, 0.0f);
    y += titelLabel->transform.scale.y + spacing;

    for (int i = 0; i < static_cast<int>(zeilen.size()); i++) {
        if (i >= scrollOffset && i < scrollOffset + MAX_SICHTBAR) {
            zeilen[i]->visible = true;
            for (auto* kind : zeilen[i]->children) kind->visible = true;
            zeilen[i]->transform.position = glm::vec3(0.0f, y, 0.0f);
            y += rowH + spacing;
        } else {
            zeilen[i]->visible = false;
            for (auto* kind : zeilen[i]->children) kind->visible = false;
        }
    }

    hinweisLabel->transform.position = glm::vec3(0.0f, y, 0.0f);
    y += hinweisLabel->transform.scale.y;

    float contentH = y;
    float contentW = 300.0f;
    float bgW = contentW + pad * 2.0f;
    float bgH = contentH + pad * 2.0f;

    glm::vec2 viewport = Kern::GetViewportSize() / UIElement::dpiScale;
    float left = (viewport.x - bgW) * 0.5f;
    float top = (viewport.y - bgH) * 0.5f;

    hintergrund->transform.position = glm::vec3(left, top, 0.0f);
    hintergrund->transform.scale = glm::vec3(bgW, bgH, 1.0f);

    container->transform.position = glm::vec3(left + pad, top + pad, 0.0f);
}

bool SchiffslaborHUD::spielerNaheKonsole() const {
    const auto& spieler = getNodesInGroup("spielerInsel");
    const auto& konsolen = getNodesInGroup("schiffslabor");
    if (spieler.empty() || konsolen.empty()) return false;

    glm::vec3 spielerPos = spieler[0]->getGlobalTransform().position;
    for (auto* k : konsolen) {
        if (glm::distance(spielerPos, k->getGlobalTransform().position) < 6.0f) {
            return true;
        }
    }
    return false;
}

void SchiffslaborHUD::umschalten(GLFWwindow* window) {
    offen = !offen;
    inventarOffen = offen;

    setzeSichtbarkeitRekursiv(this, offen);

    if (offen) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorDisabled = false;
        aktualisieren();
        scrollAktualisieren();
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

void SchiffslaborHUD::aktualisieren() {
    for (auto* zeile : zeilen) {
        zeile->aktualisieren();
    }
}

void SchiffslaborHUD::herstellen() {
    if (zeilen.empty()) return;
    auto* zeile = zeilen[ausgewaehlteZeile];
    if (!zeile->herstellbar) {
        std::cout << "[Ship Lab] Not enough ingredients!" << std::endl;
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
    std::cout << "[Ship Lab] Crafted: " << info.name << " x" << r.ausgabeAnzahl << std::endl;
    QuestManager::getInstance().progressObjective("craft_" + info.name, 1);

    if (r.ausgabe == GegenstandID::TODESSTERN && parent != nullptr) {
        parent->addChild(new QuestCompletedBanner("The galaxy trembles.", "D E A T H   S T A R   B U I L T"));
    }
}

void SchiffslaborHUD::scrollAktualisieren() {
    if (ausgewaehlteZeile < scrollOffset) {
        scrollOffset = ausgewaehlteZeile;
    } else if (ausgewaehlteZeile >= scrollOffset + MAX_SICHTBAR) {
        scrollOffset = ausgewaehlteZeile - MAX_SICHTBAR + 1;
    }
}
