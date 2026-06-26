#include "handwerkHUD.h"
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

void HandwerkHUD::onInit() {
    hintergrund = new UIElement();
    addChild(hintergrund);
    hintergrund->material.albedo = Kern::LoadTexture("assets/textures/inventar_hintergrund.png");

    container = new UIElement();
    addChild(container);

    titelLabel = new UILabel();
    titelLabel->setText("Crafting", 36.0f);
    titelLabel->color = glm::vec4(0.8f, 0.9f, 1.0f, 1.0f);
    container->addChild(titelLabel);

    rezepte = GegenstandDaten::getInstance().getRezepteFuer(RezeptQuelle::HAND);

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

void HandwerkHUD::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    const auto& walkers = getNodesInGroup("playerWalking");
    bool spielerAktiv = !walkers.empty() && dynamic_cast<Player*>(walkers[0])->getActive();

    if (Input::isKeyJustPressed(GLFW_KEY_C) && (spielerAktiv || offen) && (!inventarOffen || offen)) {
        umschalten(window);
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
    int sichtbar = glm::min(MAX_SICHTBAR, static_cast<int>(zeilen.size()));

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

void HandwerkHUD::umschalten(GLFWwindow* window) {
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

void HandwerkHUD::aktualisieren() {
    for (auto* zeile : zeilen) {
        zeile->aktualisieren();
    }
}

void HandwerkHUD::herstellen() {
    if (zeilen.empty()) return;
    auto* zeile = zeilen[ausgewaehlteZeile];
    if (!zeile->herstellbar) {
        std::cout << "[Crafting] Not enough ingredients!" << std::endl;
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
    std::cout << "[Crafting] Crafted: " << info.name << std::endl;
    QuestManager::getInstance().progressObjective("craft_" + info.name, 1);
}

void HandwerkHUD::scrollAktualisieren() {
    if (ausgewaehlteZeile < scrollOffset) {
        scrollOffset = ausgewaehlteZeile;
    } else if (ausgewaehlteZeile >= scrollOffset + MAX_SICHTBAR) {
        scrollOffset = ausgewaehlteZeile - MAX_SICHTBAR + 1;
    }
}

void RezeptZeileUI::onInit() {
    spacing = 6.0f;
    setAlignment(UIAlignment::CENTER);

    const auto& info1 = GegenstandDaten::getInstance().getInfo(rezept.eingabe1);
    const auto& info2 = GegenstandDaten::getInstance().getInfo(rezept.eingabe2);
    const auto& infoOut = GegenstandDaten::getInstance().getInfo(rezept.ausgabe);

    cursorLabel = new UILabel();
    cursorLabel->setText(">", 24.0f);
    cursorLabel->color = glm::vec4(1.0f, 0.85f, 0.3f, 1.0f);
    cursorLabel->visible = false;
    addChild(cursorLabel);

    icon1 = new UIElement();
    addChild(icon1);
    icon1->material.albedo = info1.iconTextur;
    icon1->transform.scale = glm::vec3(36.0f, 36.0f, 1.0f);

    plusLabel = new UILabel();
    plusLabel->setText("+", 22.0f);
    plusLabel->color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    addChild(plusLabel);

    icon2 = new UIElement();
    addChild(icon2);
    icon2->material.albedo = info2.iconTextur;
    icon2->transform.scale = glm::vec3(36.0f, 36.0f, 1.0f);

    pfeilLabel = new UILabel();
    pfeilLabel->setText("->", 22.0f);
    pfeilLabel->color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    addChild(pfeilLabel);

    ergebnisIcon = new UIElement();
    addChild(ergebnisIcon);
    ergebnisIcon->material.albedo = infoOut.iconTextur;
    ergebnisIcon->transform.scale = glm::vec3(36.0f, 36.0f, 1.0f);

    nameLabel = new UILabel();
    nameLabel->setText(infoOut.name, 20.0f);
    nameLabel->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    addChild(nameLabel);

    aktualisieren();
}

void RezeptZeileUI::aktualisieren() {
    const auto& inv = Inventar::getInstance();
    const auto& r = rezept;

    if (r.eingabe1 == r.eingabe2) {
        herstellbar = inv.zaehlen(r.eingabe1) >= 2;
    } else {
        herstellbar = inv.zaehlen(r.eingabe1) >= 1 && inv.zaehlen(r.eingabe2) >= 1;
    }

    glm::vec4 verfuegbar = herstellbar
        ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
        : glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

    plusLabel->color = verfuegbar * 0.7f;
    plusLabel->color.a = 1.0f;
    pfeilLabel->color = verfuegbar * 0.7f;
    pfeilLabel->color.a = 1.0f;

    if (istHervorgehoben) {
        setHervorgehoben(true);
    } else {
        nameLabel->color = verfuegbar;
    }
}

void RezeptZeileUI::setHervorgehoben(bool hervorgehoben) {
    istHervorgehoben = hervorgehoben;
    cursorLabel->visible = hervorgehoben;

    if (hervorgehoben) {
        nameLabel->color = herstellbar
            ? glm::vec4(1.0f, 0.85f, 0.3f, 1.0f)
            : glm::vec4(0.6f, 0.4f, 0.2f, 1.0f);
    } else {
        nameLabel->color = herstellbar
            ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
            : glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    }
}
