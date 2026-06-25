#include "weltraumHudPanel.h"
#include "dieWesen/feindschiff.h"
#include "szenes/weltraumszeneWesen.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/renderWerkzeuge.h"

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

    const auto abschuessReihe = new HBoxUI();
    abschuessReihe->init();
    addChild(abschuessReihe);

    abschuessLabel = new UILabel();
    abschuessLabel->setText("Kills: 0", 48.0f);
    abschuessLabel->color = glm::vec4(1.0f, 0.4f, 0.3f, 1.0f);
    abschuessLabel->init();
    abschuessReihe->addChild(abschuessLabel);

    const auto raketenReihe = new HBoxUI();
    raketenReihe->init();
    addChild(raketenReihe);

    raketenLabel = new UILabel();
    raketenLabel->setText("Rockets: 3", 48.0f);
    raketenLabel->color = glm::vec4(0.3f, 1.0f, 0.4f, 1.0f);
    raketenLabel->init();
    raketenReihe->addChild(raketenLabel);

    const auto feindeReihe = new HBoxUI();
    feindeReihe->init();
    addChild(feindeReihe);

    feindeLabel = new UILabel();
    feindeLabel->setText("Enemies: 0", 48.0f);
    feindeLabel->color = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f);
    feindeLabel->init();
    feindeReihe->addChild(feindeLabel);

    schadenVignette = new UIElement();
    parent->addChild(schadenVignette);
    schadenVignette->material.albedo = Kern::LoadTexture("assets/textures/schaden_vignette.png", true);
    schadenVignette->visible = false;

    bossLabel = new UILabel();
    bossLabel->setText("BOSS", 36.0f);
    bossLabel->color = glm::vec4(1.0f, 0.3f, 0.2f, 1.0f);
    parent->addChild(bossLabel);
    bossLabel->visible = false;

    bossBalkenHG = new UIElement();
    parent->addChild(bossBalkenHG);
    bossBalkenHG->material.albedo = Kern::LoadTexture("assets/textures/inventar_hintergrund.png");
    bossBalkenHG->visible = false;

    bossBalken = new UIElement();
    parent->addChild(bossBalken);
    bossBalken->material.albedo = Kern::LoadTexture("assets/textures/laser_rot.png");
    bossBalken->visible = false;

    texCyan = Kern::LoadTexture("assets/textures/emission_cyan.png");
    texRot = Kern::LoadTexture("assets/textures/laser_rot.png");
    texLila = Kern::LoadTexture("assets/textures/emission_lila.png");

    kompassHG = new UIElement();
    parent->addChild(kompassHG);
    kompassHG->material.albedo = Kern::LoadTexture("assets/textures/kompass_hg.png", true);

    for (int i = 0; i < KOMPASS_MAX; i++) {
        kompassPunkte[i] = new UIElement();
        parent->addChild(kompassPunkte[i]);
        kompassPunkte[i]->material.albedo = Kern::LoadTexture("assets/textures/kompass_pfeil.png", true);
        kompassPunkte[i]->visible = false;
    }
}

void WeltraumHudPanel::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    VBoxUI::onUpdate(window, deltaTime, cameraTransform);

    if (raumschiff != nullptr) {
        hpLabel->text = std::to_string(static_cast<int>(raumschiff->getLeben()));
        const float tempo = raumschiff->getGeschwindigkeit() * 3.6f;
        geschwindigkeitLabel->text = std::to_string(static_cast<int>(tempo)) + "km/h";
        raketenLabel->text = "Rockets: " + std::to_string(raumschiff->getRaketenMunition());

        if (raumschiff->schadenBlitz > 0.0f) {
            schadenVignette->visible = true;
            raumschiff->schadenBlitz -= deltaTime * 3.0f;
            if (raumschiff->schadenBlitz <= 0.0f) {
                raumschiff->schadenBlitz = 0.0f;
                schadenVignette->visible = false;
            }
        }

        glm::vec2 viewport = Kern::GetViewportSize() / UIElement::dpiScale;
        schadenVignette->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        schadenVignette->transform.scale = glm::vec3(viewport.x, viewport.y, 1.0f);
    }

    abschuessLabel->text = "Kills: " + std::to_string(WeltraumszeneWesen::getAbschuesse());
    feindeLabel->text = "Enemies: " + std::to_string(static_cast<int>(getNodesInGroup("feinde").size()));

    const auto& feinde = getNodesInGroup("feinde");
    Feindschiff* boss = nullptr;
    for (auto* f : feinde) {
        auto* fs = dynamic_cast<Feindschiff*>(f);
        if (fs && fs->transform.scale.x > 50.0f) {
            boss = fs;
            break;
        }
    }

    if (boss != nullptr) {
        glm::vec2 viewport = Kern::GetViewportSize() / UIElement::dpiScale;
        float barW = 300.0f;
        float barH = 20.0f;
        float barX = (viewport.x - barW) * 0.5f;
        float barY = viewport.y - 60.0f;
        float hpRatio = glm::max(boss->leben / 20.0f, 0.0f);

        bossLabel->visible = true;
        bossLabel->transform.position = glm::vec3(barX, barY - 35.0f, 0.0f);

        bossBalkenHG->visible = true;
        bossBalkenHG->transform.position = glm::vec3(barX, barY, 0.0f);
        bossBalkenHG->transform.scale = glm::vec3(barW, barH, 1.0f);

        bossBalken->visible = true;
        bossBalken->transform.position = glm::vec3(barX, barY, 0.0f);
        bossBalken->transform.scale = glm::vec3(barW * hpRatio, barH, 1.0f);
    } else {
        bossLabel->visible = false;
        bossBalkenHG->visible = false;
        bossBalken->visible = false;
    }

    kompassAktualisieren();
}

void WeltraumHudPanel::kompassAktualisieren() {
    glm::vec2 viewport = Kern::GetViewportSize() / UIElement::dpiScale;
    float kompassX = (viewport.x - KOMPASS_BREITE) * 0.5f;
    float kompassY = 10.0f;
    float kompassH = 32.0f;
    float kompassMitte = kompassX + KOMPASS_BREITE * 0.5f;

    kompassHG->transform.position = glm::vec3(kompassX, kompassY, 0.0f);
    kompassHG->transform.scale = glm::vec3(KOMPASS_BREITE, kompassH, 1.0f);

    if (raumschiff == nullptr) return;

    glm::vec3 spielerPos = raumschiff->getGlobalTransform().position;
    glm::vec3 vorwaerts = raumschiff->transform.forward();
    glm::vec3 rechts = raumschiff->transform.right();

    const auto& feinde = getNodesInGroup("feinde");
    int punktIndex = 0;

    for (auto* f : feinde) {
        if (punktIndex >= KOMPASS_MAX) break;

        glm::vec3 feindPos = f->getGlobalTransform().position;
        glm::vec3 delta = feindPos - spielerPos;
        float abstand = glm::length(delta);
        if (abstand < 1.0f) { continue; }

        glm::vec3 richtung = delta / abstand;

        float vorne = glm::dot(richtung, vorwaerts);
        float seite = glm::dot(richtung, rechts);
        float winkel = glm::atan(seite, vorne);

        float halbeBreite = KOMPASS_BREITE * 0.5f - 10.0f;
        float xPos = glm::clamp(winkel / 3.14159f * halbeBreite, -halbeBreite, halbeBreite);

        float dy = feindPos.y - spielerPos.y;
        float hoehenFaktor = glm::clamp(dy / 60.0f, -1.0f, 1.0f);

        bool istBoss = f->transform.scale.x > 50.0f;
        float basisGroesse = istBoss ? 20.0f : 12.0f;
        float groesse = basisGroesse + hoehenFaktor * 4.0f;

        auto* punkt = kompassPunkte[punktIndex];
        punkt->visible = true;

        if (istBoss) {
            punkt->material.albedo = texRot;
        } else {
            auto* fs = dynamic_cast<Feindschiff*>(f);
            if (fs) {
                switch (fs->typ) {
                    case FeindTyp::PATROUILLE: punkt->material.albedo = texCyan; break;
                    case FeindTyp::JAEGER:     punkt->material.albedo = texRot; break;
                    case FeindTyp::ORBITER:    punkt->material.albedo = texLila; break;
                }
            }
        }

        punkt->transform.scale = glm::vec3(groesse, groesse, 1.0f);
        punkt->transform.position = glm::vec3(
            kompassMitte + xPos - groesse * 0.5f,
            kompassY + (kompassH - groesse) * 0.5f,
            0.0f
        );

        punktIndex++;
    }

    for (int i = punktIndex; i < KOMPASS_MAX; i++) {
        kompassPunkte[i]->visible = false;
    }
}
