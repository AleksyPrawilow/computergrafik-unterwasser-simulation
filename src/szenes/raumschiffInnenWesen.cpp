//
// Created by Kajetan on 26/06/2026.
//

#include "raumschiffInnenWesen.h"

#include "dieWesen/player.h"
#include "dieWesen/alien.h"
#include "dieWesen/feindschiff.h"
#include "dieWesen/aufhebbar.h"
#include "dieWesen/platzierbaresObjekt.h"
#include "dieWesen/ui/inventarHUD.h"
#include "dieWesen/ui/handwerkHUD.h"
#include "dieWesen/ui/ausruestungsLeiste.h"
#include "dieWesen/ui/schiffslaborHUD.h"
#include "dieWesen/ui/fadeOverlay.h"
#include "dieWesen/ui/cinematicBars.h"
#include "dieWesen/ui/questCompletedBanner.h"
#include "dieWesen/explosion.h"
#include "dieWesen/lightsaber.h"
#include "werkzeuge/himmelsboxWesen.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/random.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/renderWerkzeuge.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/ui/uiContainers.h"
#include "werkzeuge/audio/musicManager.h"
#include "werkzeuge/visual/worldEnvironment.h"
#include "werkzeuge/visual/tween.h"
#include "sceneManager.h"

extern Kamera kamera;

int RaumschiffInnenWesen::cutscenePhase = 0;

namespace {
    constexpr float WAND_HOEHE = 8.0f;

    Wesen* makeBoden(const glm::vec3& center, const glm::vec3& size,
                     GLuint albedo, GLuint normal, GLuint roughness) {
        auto* boden = new Wesen();
        boden->loadModel("assets/models/cube.obj");
        boden->material.albedo = albedo;
        boden->material.normal = normal;
        boden->material.roughness = roughness;
        boden->material.shader = ShaderManager::getInstance().getShader("default");
        boden->transform.position = center;
        boden->transform.scale = size * 0.5f; // cube spans -1..1 => 2 units
        return boden;
    }
}

void RaumschiffInnenWesen::init() {
    addChild(new HimmelsboxWesen({
        "assets/textures/skybox_weltraum/px.png",
        "assets/textures/skybox_weltraum/nx.png",
        "assets/textures/skybox_weltraum/py.png",
        "assets/textures/skybox_weltraum/ny.png",
        "assets/textures/skybox_weltraum/pz.png",
        "assets/textures/skybox_weltraum/nz.png"
    }));

    auto * umwelt = new WorldEnvironment();
    umwelt->init();
    umwelt->params.sunEnergy = 2.5f;
    umwelt->params.sunDirection = glm::normalize(glm::vec3(0.2f, 0.9f, 0.3f));
    umwelt->params.ambientColor = glm::vec3(0.3f, 0.32f, 0.4f);
    umwelt->params.ambientEnergy = 0.25f;
    umwelt->params.fogEnabled = false;
    umwelt->params.heightFogEnabled = false;
    umwelt->params.causticsEnabled = false;
    umwelt->params.depthDimmingEnabled = false;
    umwelt->params.bloomThreshold = 0.0f;
    umwelt->params.bloomIntensity = 1.2f;
    addChild(umwelt);

    GLuint bodenAlbedo = Kern::LoadTexture("assets/textures/platform_albedo.png");
    GLuint bodenNormal = Kern::LoadTexture("assets/textures/platform_normal.png");
    GLuint bodenRough = Kern::LoadTexture("assets/textures/platform_roughness.png");
    GLuint wandAlbedo = Kern::LoadTexture("assets/textures/metall_wand.png");
    GLuint wandNormal = Kern::LoadTexture("assets/textures/metall_wand_normal.png");
    GLuint wandRough = Kern::LoadTexture("assets/textures/RockTexture001_metallic.png");

    // Footprint: X [-33, 33], Z [-41, 25]. Floor top at Y = 0, ends at the window plane (Z=-41).
    addChild(makeBoden(glm::vec3(0.0f, -0.5f, -7.0f), glm::vec3(70.0f, 1.0f, 68.0f),
                       bodenAlbedo, bodenNormal, bodenRough));
    addChild(makeBoden(glm::vec3(0.0f, WAND_HOEHE + 0.5f, -7.0f), glm::vec3(70.0f, 1.0f, 68.0f),
                       bodenAlbedo, bodenNormal, bodenRough));

    constexpr float T = 2.5f;            // wall thickness (thick enough to stop the player's
                                         // centre-point collision before the camera clips in)
    constexpr float DB = 6.0f;           // doorway width

    // --- General box builder. collidable -> joins "hauswand"; visible -> rendered. ---
    auto baueWesen = [&](const glm::vec3& center, const glm::vec3& size, bool collidable, bool sichtbar) {
        auto* w = new Wesen();
        w->loadModel("assets/models/cube.obj");
        w->material.albedo = wandAlbedo;
        w->material.normal = wandNormal;
        w->material.roughness = wandRough;
        w->material.shader = ShaderManager::getInstance().getShader("default");
        w->transform.position = center;
        w->transform.scale = size * 0.5f;
        w->visible = sichtbar;
        if (collidable) {
            w->isCollidable = true;
            w->addToGroup("hauswand");
        }
        addChild(w);
    };

    // Full-height collidable wall, given an XZ centre at floor level.
    auto makeWand = [&](const glm::vec3& center, const glm::vec3& size) {
        baueWesen(center + glm::vec3(0.0f, WAND_HOEHE * 0.5f, 0.0f), size, true, true);
    };

    // Wall running along X at fixed Z (segment x1..x2). Skips zero-length pieces.
    auto seg = [&](float z, float x1, float x2) {
        if (x2 - x1 < 0.05f) return;
        makeWand(glm::vec3((x1 + x2) * 0.5f, 0.0f, z), glm::vec3(x2 - x1, WAND_HOEHE, T));
    };
    // Wall running along Z at fixed X (segment z1..z2).
    auto segZ = [&](float x, float z1, float z2) {
        if (z2 - z1 < 0.05f) return;
        makeWand(glm::vec3(x, 0.0f, (z1 + z2) * 0.5f), glm::vec3(T, WAND_HOEHE, z2 - z1));
    };
    // Wall along X with an optional centered doorway.
    auto wandX = [&](float z, float x1, float x2, bool tuer, float tM) {
        if (!tuer) { seg(z, x1, x2); return; }
        seg(z, x1, tM - DB * 0.5f);
        seg(z, tM + DB * 0.5f, x2);
    };
    // Wall along Z with an optional centered doorway.
    auto wandZ = [&](float x, float z1, float z2, bool tuer, float tM) {
        if (!tuer) { segZ(x, z1, z2); return; }
        segZ(x, z1, tM - DB * 0.5f);
        segZ(x, tM + DB * 0.5f, z2);
    };

    // === Hull (outer shell) ===
    segZ(-33.0f, -41.0f, 3.0f);          // west hull (engine + bridge)
    segZ(33.0f, -41.0f, 3.0f);           // east hull (cargo + bridge)
    wandX(25.0f, -11.0f, 11.0f, false, 0.0f);  // south hull (entry back)
    segZ(-11.0f, 3.0f, 25.0f);           // entry west hull
    segZ(11.0f, 3.0f, 25.0f);            // entry east hull

    // === Bridge windows (north wall at Z=-41) looking into space ===
    // Invisible full-height collision barrier so the player can't walk into space.
    baueWesen(glm::vec3(0.0f, WAND_HOEHE * 0.5f, -41.0f), glm::vec3(66.0f, WAND_HOEHE, T), true, false);
    // Visible frame: sill (Y 0..2) and header (Y 6..8), full width.
    baueWesen(glm::vec3(0.0f, 1.0f, -41.0f), glm::vec3(66.0f, 2.0f, T), false, true);
    baueWesen(glm::vec3(0.0f, 7.0f, -41.0f), glm::vec3(66.0f, 2.0f, T), false, true);
    // Vertical mullions dividing the glass band (Y 2..6) into panes.
    for (float mx : {-22.0f, -11.0f, 11.0f, 22.0f}) {
        baueWesen(glm::vec3(mx, 4.0f, -41.0f), glm::vec3(1.5f, 4.0f, T), false, true);
    }

    // === Internal dividers ===
    // Bridge-south wall at Z=-19. Doorways only at engine (X=-22) and cargo (X=22) sides;
    // the centre is SOLID so the bridge/console is hidden from the entry sightline.
    seg(-19.0f, -33.0f, -25.0f);
    seg(-19.0f, -19.0f, 19.0f);
    seg(-19.0f, 25.0f, 33.0f);
    // Row-south wall at Z=3: engine/cargo solid, hub has door to entry
    wandX(3.0f, -33.0f, -11.0f, false, 0.0f);
    wandX(3.0f, -11.0f, 11.0f, true, 0.0f);
    wandX(3.0f, 11.0f, 33.0f, false, 0.0f);
    // Engine|Hub divider (X=-11) and Cargo|Hub divider (X=11), each with a door at Z=-8
    wandZ(-11.0f, -19.0f, 3.0f, true, -8.0f);
    wandZ(11.0f, -19.0f, 3.0f, true, -8.0f);

    // === Decorative pillars at doorways and junctions (non-collidable, just visual breakup) ===
    GLuint bridgeAlbedo = Kern::LoadTexture("assets/textures/bridge_albedo.png");
    GLuint bridgeNormal = Kern::LoadTexture("assets/textures/bridge_normal.png");
    GLuint bridgeRough = Kern::LoadTexture("assets/textures/bridge_roughness.png");

    auto makePillar = [&](const glm::vec3& pos) {
        auto* p = new Wesen();
        p->loadModel("assets/models/cube.obj");
        p->material.albedo = wandAlbedo;
        p->material.normal = wandNormal;
        p->material.roughness = wandRough;
        p->material.shader = ShaderManager::getInstance().getShader("default");
        p->transform.position = pos + glm::vec3(0.0f, WAND_HOEHE * 0.5f, 0.0f);
        p->transform.scale = glm::vec3(1.0f, WAND_HOEHE * 0.5f, 1.0f);
        p->isCollidable = true;
        p->addToGroup("hauswand");
        addChild(p);
    };

    // Hub-to-entry doorway pillars
    makePillar(glm::vec3(-3.0f, 0.0f, 3.0f));
    makePillar(glm::vec3(3.0f, 0.0f, 3.0f));
    // Engine room door pillars
    makePillar(glm::vec3(-11.0f, 0.0f, -5.0f));
    makePillar(glm::vec3(-11.0f, 0.0f, -11.0f));
    // Cargo room door pillars
    makePillar(glm::vec3(11.0f, 0.0f, -5.0f));
    makePillar(glm::vec3(11.0f, 0.0f, -11.0f));
    // Bridge doorway pillars (engine side)
    makePillar(glm::vec3(-25.0f, 0.0f, -19.0f));
    makePillar(glm::vec3(-19.0f, 0.0f, -19.0f));
    // Bridge doorway pillars (cargo side)
    makePillar(glm::vec3(19.0f, 0.0f, -19.0f));
    makePillar(glm::vec3(25.0f, 0.0f, -19.0f));
    // Entry room corner pillars
    makePillar(glm::vec3(-11.0f, 0.0f, 25.0f));
    makePillar(glm::vec3(11.0f, 0.0f, 25.0f));
    // Bridge far corners
    makePillar(glm::vec3(-33.0f, 0.0f, -41.0f));
    makePillar(glm::vec3(33.0f, 0.0f, -41.0f));

    // === Ceiling light strips (glowing emissive bars running along corridors) ===
    GLuint lichtTex = Kern::LoadTexture("assets/textures/emission_cyan.png");
    auto makeLicht = [&](const glm::vec3& pos, const glm::vec3& size) {
        auto* l = new Wesen();
        l->loadModel("assets/models/cube.obj");
        l->material.albedo = lichtTex;
        l->material.emission = lichtTex;
        l->material.bloomStrength = 0.7f;
        l->material.shader = ShaderManager::getInstance().getShader("default");
        l->transform.position = pos;
        l->transform.scale = size * 0.5f;
        addChild(l);
    };
    // Entry corridor ceiling strips
    makeLicht(glm::vec3(0.0f, WAND_HOEHE - 0.3f, 14.0f), glm::vec3(0.4f, 0.3f, 20.0f));
    // Hub ceiling strip
    makeLicht(glm::vec3(0.0f, WAND_HOEHE - 0.3f, -8.0f), glm::vec3(0.4f, 0.3f, 18.0f));
    // Engine room strip
    makeLicht(glm::vec3(-22.0f, WAND_HOEHE - 0.3f, -8.0f), glm::vec3(0.4f, 0.3f, 18.0f));
    // Cargo bay strip
    makeLicht(glm::vec3(22.0f, WAND_HOEHE - 0.3f, -8.0f), glm::vec3(0.4f, 0.3f, 18.0f));
    // Bridge strips (four parallel lines)
    makeLicht(glm::vec3(-16.0f, WAND_HOEHE - 0.3f, -30.0f), glm::vec3(0.4f, 0.3f, 20.0f));
    makeLicht(glm::vec3(-6.0f, WAND_HOEHE - 0.3f, -30.0f), glm::vec3(0.4f, 0.3f, 20.0f));
    makeLicht(glm::vec3(6.0f, WAND_HOEHE - 0.3f, -30.0f), glm::vec3(0.4f, 0.3f, 20.0f));
    makeLicht(glm::vec3(16.0f, WAND_HOEHE - 0.3f, -30.0f), glm::vec3(0.4f, 0.3f, 20.0f));

    // --- Inventory reset: clear everything, equip photon blaster in slot 1 ---
    Inventar::getInstance().reset();
    Inventar::getInstance().hinzufuegen(GegenstandID::PHOTONENBLASTER, 1);
    Inventar::getInstance().ausruesten(0, GegenstandID::PHOTONENBLASTER);

    // --- Player (spawn in entry, facing north toward the hub) ---
    glm::vec3 spielerStart = glm::vec3(0.0f, 3.0f, 18.0f);
    auto * player = new Player();
    player->affectedByWater = false;
    player->benutzeInsel = false;
    player->bodenHoehe = 0.0f;
    player->targetY = 3.0f;
    player->spawnPosition = spielerStart;
    addChild(player);
    player->transform.position = spielerStart;

    // --- Lab console (centre of the bridge) ---
    auto * konsole = new PlatzierbaresObjekt(GegenstandID::LABORKONSOLE);
    konsole->transform.position = glm::vec3(0.0f, 1.2f, -30.0f);
    addChild(konsole);

    // --- Scattered collectibles across the rooms ---
    struct Pickup { GegenstandID id; int anzahl; glm::vec3 pos; };
    const Pickup pickups[] = {
        { GegenstandID::METALLSCHROTT, 2, glm::vec3(-6.0f, 1.0f, 12.0f) }, // entry
        { GegenstandID::SCHALTKREIS,   1, glm::vec3(6.0f, 1.0f, 12.0f) },  // entry
        { GegenstandID::METALLSCHROTT, 1, glm::vec3(0.0f, 1.0f, -8.0f) },  // hub
        { GegenstandID::METALLSCHROTT, 2, glm::vec3(-26.0f, 1.0f, -8.0f) },// engine room
        { GegenstandID::SCHALTKREIS,   1, glm::vec3(-22.0f, 1.0f, -14.0f) },// engine room
        { GegenstandID::SCHALTKREIS,   2, glm::vec3(26.0f, 1.0f, -8.0f) }, // cargo bay
        { GegenstandID::METALLSCHROTT, 1, glm::vec3(22.0f, 1.0f, 0.0f) },  // cargo bay
        { GegenstandID::SCHALTKREIS,   1, glm::vec3(-20.0f, 1.0f, -34.0f) },// bridge
        { GegenstandID::METALLSCHROTT, 1, glm::vec3(20.0f, 1.0f, -34.0f) },// bridge
    };
    for (const auto& p : pickups) {
        auto * item = new Aufhebbar(p.id, p.anzahl);
        item->transform.position = p.pos;
        addChild(item);
    }

    // --- Real enemy ships patrolling space outside the bridge windows ---
    // They use the space-scene Feindschiff: fly between waypoints and fire. The walking
    // player isn't in the "spieler" group, so their chase-AI falls back to patrol and
    // their lasers fly off harmlessly -> a distant skirmish framed by the windows.
    struct Patrouille { glm::vec3 zentrum; float radius; float scale; float tempo; };
    const Patrouille schiffe[] = {
        { glm::vec3(-25.0f, 6.0f, -110.0f), 30.0f, 8.0f, 7.0f },
        { glm::vec3(30.0f, -6.0f, -130.0f), 45.0f, 11.0f, 9.0f },
        { glm::vec3(0.0f, 14.0f, -165.0f), 60.0f, 14.0f, 11.0f },
        { glm::vec3(-45.0f, -8.0f, -145.0f), 40.0f, 9.0f, 8.0f },
        { glm::vec3(50.0f, 18.0f, -180.0f), 55.0f, 12.0f, 10.0f },
    };
    for (const auto& s : schiffe) {
        auto * feind = new Feindschiff();
        feind->typ = FeindTyp::PATROUILLE;
        for (int w = 0; w < 4; w++) {
            float a = static_cast<float>(w) * 1.5708f;
            feind->wegpunkte.emplace_back(s.zentrum + glm::vec3(
                glm::cos(a) * s.radius,
                glm::sin(a * 2.0f) * s.radius * 0.2f,
                glm::sin(a) * s.radius));
        }
        feind->bewegungsGeschwindigkeit = s.tempo;
        feind->schussIntervall = Random::range(1.5f, 3.0f);
        addChild(feind);
        feind->transform.position = feind->wegpunkte[0];
        feind->transform.scale = glm::vec3(s.scale);
        feind->boundingRadius = s.scale;
        feind->material.emission = Kern::LoadTexture("assets/textures/laser_rot.png");
        feind->material.bloomStrength = 0.12f;
    }

    // --- Aliens patrolling corridors ---
    struct AlienSpawn { glm::vec3 a, b; };
    const AlienSpawn alienSpawns[] = {
        { glm::vec3(0.0f, 0.0f, -4.0f),   glm::vec3(0.0f, 0.0f, -16.0f) },   // hub corridor
        { glm::vec3(-22.0f, 0.0f, -2.0f),  glm::vec3(-22.0f, 0.0f, -16.0f) }, // engine room
        { glm::vec3(22.0f, 0.0f, -2.0f),   glm::vec3(22.0f, 0.0f, -16.0f) },  // cargo bay
        { glm::vec3(-20.0f, 0.0f, -28.0f), glm::vec3(20.0f, 0.0f, -28.0f) },  // bridge front
        { glm::vec3(-10.0f, 0.0f, -36.0f), glm::vec3(10.0f, 0.0f, -36.0f) },  // bridge back
    };
    for (const auto& as : alienSpawns) {
        auto* alien = new Alien();
        alien->wegpunkte.push_back(as.a);
        alien->wegpunkte.push_back(as.b);
        addChild(alien);
        alien->transform.position = as.a + glm::vec3(0.0f, 1.5f, 0.0f);
    }

    // --- Health HUD (top-left) ---
    auto* lebenHUD = new VBoxUI(8.0f);
    addChild(lebenHUD);
    lebenHUD->transform.position = glm::vec3(80.0f, 20.0f, 0.0f);

    auto* lebenReihe = new HBoxUI();
    lebenReihe->init();
    lebenHUD->addChild(lebenReihe);

    auto* herzIcon = new UIElement();
    herzIcon->init();
    herzIcon->material.albedo = Kern::LoadTexture("assets/textures/heart.png", true);
    herzIcon->transform.scale = glm::vec3(48.0f, 48.0f, 1.0f);
    lebenReihe->addChild(herzIcon);

    lebenLabel = new UILabel();
    lebenLabel->setText("100", 48.0f);
    lebenLabel->init();
    lebenReihe->addChild(lebenLabel);

    // --- Damage vignette ---
    schadenVignette = new UIElement();
    addChild(schadenVignette);
    schadenVignette->material.albedo = Kern::LoadTexture("assets/textures/schaden_vignette.png", true);
    schadenVignette->visible = false;

    spielerRef = player;

    // --- Enemy compass bar ---
    kompassHG = new UIElement();
    addChild(kompassHG);
    kompassHG->material.albedo = Kern::LoadTexture("assets/textures/kompass_hg.png", true);

    GLuint punktTex = Kern::LoadTexture("assets/textures/emission_lila.png");
    for (int i = 0; i < KOMPASS_MAX; i++) {
        kompassPunkte[i] = new UIElement();
        addChild(kompassPunkte[i]);
        kompassPunkte[i]->material.albedo = punktTex;
        kompassPunkte[i]->transform.scale = glm::vec3(10.0f, 10.0f, 1.0f);
        kompassPunkte[i]->visible = false;
    }

    // --- Cutscene support ---
    cinematicBars = new CinematicBars();
    addChild(cinematicBars);

    fadeOverlay = new FadeOverlay();
    addChild(fadeOverlay);
    fadeOverlay->sofort(1.0f);
    fadeOverlay->fadeOut(1.0f);

    // --- Death Star callback ---
    player->onTodessternBenutzt = [this]() {
        if (!cutsceneGestartet) starteCutscene();
    };

    // --- HUDs ---
    addChild(new InventarHUD());
    addChild(new HandwerkHUD());
    addChild(new SchiffslaborHUD());
    addChild(new AusruestungsLeiste());

    MusicManager::getInstance().playMusic("assets/audio/beatit.mp3", 1.5f, true);

    if (cutscenePhase == 2) {
        cutscenePhase = 0;
        startePhase2();
    }
}

void RaumschiffInnenWesen::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (spielerRef != nullptr) {
        lebenLabel->text = std::to_string(static_cast<int>(spielerRef->getLeben()));

        if (spielerRef->schadenBlitz > 0.0f) {
            schadenVignette->visible = true;
        } else {
            schadenVignette->visible = false;
        }

        glm::vec2 viewport = Kern::GetViewportSize() / UIElement::dpiScale;
        schadenVignette->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        schadenVignette->transform.scale = glm::vec3(viewport.x, viewport.y, 1.0f);
    }

    if (cutsceneAktiv) {
        const float tFolge = 1.0f - glm::exp(-3.0f * deltaTime);
        cameraTransform.position = glm::mix(cameraTransform.position, cameraZielPos, tFolge);
        cameraTransform.lookAt(cameraBlickZiel, glm::vec3(0.0f, 1.0f, 0.0f));

        if (leviathanModell && glm::length2(leviathanEuler) > 0.001f) {
            leviathanModell->transform.rotation = glm::quat(glm::radians(leviathanEuler));
        }
    }

    kompassAktualisieren();
}

void RaumschiffInnenWesen::kompassAktualisieren() {
    glm::vec2 viewport = Kern::GetViewportSize() / UIElement::dpiScale;
    float kompassX = (viewport.x - KOMPASS_BREITE) * 0.5f;
    float kompassY = 10.0f;
    float kompassH = 28.0f;
    float kompassMitte = kompassX + KOMPASS_BREITE * 0.5f;

    kompassHG->transform.position = glm::vec3(kompassX, kompassY, 0.0f);
    kompassHG->transform.scale = glm::vec3(KOMPASS_BREITE, kompassH, 1.0f);

    if (spielerRef == nullptr) return;

    glm::vec3 spielerPos = spielerRef->getGlobalTransform().position;
    glm::vec3 vorwaerts = spielerRef->transform.forward();
    vorwaerts.y = 0.0f;
    if (glm::length2(vorwaerts) > 0.001f) vorwaerts = glm::normalize(vorwaerts);
    glm::vec3 rechts = spielerRef->transform.right();
    rechts.y = 0.0f;
    if (glm::length2(rechts) > 0.001f) rechts = glm::normalize(rechts);

    const auto& aliens = getNodesInGroup("aliens");
    int idx = 0;

    for (auto* a : aliens) {
        if (idx >= KOMPASS_MAX) break;

        glm::vec3 delta = a->getGlobalTransform().position - spielerPos;
        delta.y = 0.0f;
        float dist = glm::length(delta);
        if (dist < 0.5f) continue;

        glm::vec3 dir = delta / dist;
        float vorne = glm::dot(dir, vorwaerts);
        float seite = glm::dot(dir, rechts);
        float winkel = glm::atan(seite, vorne);

        float halbeBreite = KOMPASS_BREITE * 0.5f - 8.0f;
        float xPos = glm::clamp(winkel / 3.14159f * halbeBreite, -halbeBreite, halbeBreite);

        auto* punkt = kompassPunkte[idx];
        punkt->visible = true;
        punkt->transform.position = glm::vec3(
            kompassMitte + xPos - 5.0f,
            kompassY + (kompassH - 10.0f) * 0.5f,
            0.0f
        );
        idx++;
    }

    for (int i = idx; i < KOMPASS_MAX; i++) {
        kompassPunkte[i]->visible = false;
    }
}

void RaumschiffInnenWesen::starteCutscene() {
    cutsceneGestartet = true;

    if (spielerRef) spielerRef->setActive(false);
    cinematicBars->setEnabled(true);

    auto feinde = getNodesInGroup("feinde");
    for (auto* f : feinde) f->queueDestroy();
    auto feindlaser = getNodesInGroup("feindlaser");
    for (auto* l : feindlaser) l->queueDestroy();
    auto aliens = getNodesInGroup("aliens");
    for (auto* a : aliens) a->queueDestroy();

    // Phase 1: fade to black, switch to underwater for the leviathan part
    cutscenePhase = 1;
    fadeOverlay->fadeIn(1.0f, []() {
        Scene::requestSceneSwitch(0);
    });
}

void RaumschiffInnenWesen::startePhase2() {
    cutsceneAktiv = true;
    cutsceneGestartet = true;

    cinematicBars = new CinematicBars();
    addChild(cinematicBars);
    cinematicBars->setEnabled(true);

    if (spielerRef) spielerRef->setActive(false);

    auto feinde = getNodesInGroup("feinde");
    for (auto* f : feinde) f->queueDestroy();

    // Death Star at the window
    todessternKugel = new Wesen();
    todessternKugel->loadModel("assets/models/death_star.obj");
    todessternKugel->material.albedo = Kern::LoadTexture("assets/textures/death_star/material.png");
    todessternKugel->material.emission = Kern::LoadTexture("assets/textures/death_star/emissiveMap1.png");
    todessternKugel->material.normal = Kern::LoadTexture("assets/textures/death_star/normalMap1.png");
    todessternKugel->material.metallic = Kern::LoadTexture("assets/textures/death_star/metalnessMap1.png");
    todessternKugel->material.bloomStrength = 0.4f;
    todessternKugel->material.shader = ShaderManager::getInstance().getShader("default");
    todessternKugel->transform.position = glm::vec3(0.0f, 4.0f, -42.0f);
    todessternKugel->transform.scale = glm::vec3(0.04f);
    addChild(todessternKugel);

    // Leviathan already in space
    leviathanModell = new Wesen();
    leviathanModell->loadModel("assets/models/leviathan.obj");
    leviathanModell->material.albedo = Kern::LoadTexture("assets/textures/leviathan_albedo.png");
    leviathanModell->material.emission = Kern::LoadTexture("assets/textures/leviathan_emissive.png");
    leviathanModell->material.normal = Kern::LoadTexture("assets/textures/leviathan_normal.png");
    leviathanModell->material.bloomStrength = 0.3f;
    leviathanModell->material.shader = ShaderManager::getInstance().getShader("default");
    leviathanModell->transform.position = glm::vec3(120.0f, 10.0f, -450.0f);
    leviathanModell->transform.scale = glm::vec3(6.0f);
    addChild(leviathanModell);

    const std::string saberFarben[4] = {"red", "purple", "green", "blue"};
    glm::vec3 saberPositionen[4] = {
        glm::vec3( 6.0f,  4.0f, 22.0f),
        glm::vec3( 8.0f, -6.0f, 18.0f),
        glm::vec3(-6.0f,  4.0f, 22.0f),
        glm::vec3(-8.0f, -6.0f, 18.0f),
    };
    for (int i = 0; i < 4; i++) {
        auto* dummy = new Wesen();
        auto* saber = new Lightsaber("assets/textures/lightsaber_" + saberFarben[i] + ".png");
        saber->dir = (i % 2 == 0) ? 1 : -1;
        leviathanModell->addChild(dummy);
        dummy->addChild(saber);
        dummy->transform.position = saberPositionen[i];
    }

    cameraZielPos = glm::vec3(0.0f, 4.0f, -30.0f);
    cameraBlickZiel = glm::vec3(-20.0f, 4.0f, -300.0f);

    // Fade in from black, then Death Star sequence
    fadeOverlay = new FadeOverlay();
    addChild(fadeOverlay);
    fadeOverlay->sofort(1.0f);
    fadeOverlay->fadeOut(1.0f);

    createTween()
        ->tweenInterval(1.5f)
        // Death Star flies out and grows
        ->tweenProperty(&todessternKugel->transform.position.z, -300.0f, 4.0f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&todessternKugel->transform.position.x, -60.0f, 4.0f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&todessternKugel->transform.scale.x, 1.2f, 4.0f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&todessternKugel->transform.scale.y, 1.2f, 4.0f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&todessternKugel->transform.scale.z, 1.2f, 4.0f, EaseType::EASE_OUT_CUBIC)
        ->tweenInterval(1.5f)
        ->tweenCallback([this]() { spawnRiesenLaser(); })
        ->tweenInterval(0.8f)
        ->tweenCallback([this]() {
            if (leviathanModell) {
                addChild(new Explosion(leviathanModell->getGlobalTransform().position, 40.0f));
                addChild(new Explosion(leviathanModell->getGlobalTransform().position + glm::vec3(10.0f, 5.0f, -5.0f), 25.0f));
                addChild(new Explosion(leviathanModell->getGlobalTransform().position + glm::vec3(-15.0f, -3.0f, 8.0f), 30.0f));
                leviathanModell->queueDestroy();
                leviathanModell = nullptr;
            }
            if (riesenLaser) {
                riesenLaser->queueDestroy();
                riesenLaser = nullptr;
            }
        })
        ->tweenInterval(2.0f)
        ->tweenCallback([this]() {
            fadeOverlay->fadeIn(2.0f, [this]() {
                addChild(new QuestCompletedBanner("The End.", "Y O U   W I N"));
            });
        });
}

void RaumschiffInnenWesen::spawnRiesenLaser() {
    if (!todessternKugel || !leviathanModell) return;

    glm::vec3 von = todessternKugel->getGlobalTransform().position;
    glm::vec3 nach = leviathanModell->getGlobalTransform().position;
    glm::vec3 richtung = nach - von;
    float laenge = glm::length(richtung);
    if (laenge < 1.0f) return;

    glm::vec3 mitte = (von + nach) * 0.5f;
    glm::quat rot = Transform::quatLookAt(glm::normalize(richtung), glm::vec3(0.0f, 1.0f, 0.0f));

    auto* laser = new Wesen();
    laser->loadModel("assets/models/cube.obj");
    laser->material.albedo = Kern::LoadTexture("assets/textures/emission_gruen.png");
    laser->material.emission = Kern::LoadTexture("assets/textures/emission_gruen.png");
    laser->material.bloomStrength = 1.5f;
    laser->material.shader = ShaderManager::getInstance().getShader("default");
    laser->transform.position = mitte;
    laser->transform.rotation = rot;
    laser->transform.scale = glm::vec3(3.0f, 3.0f, laenge * 0.5f);
    addChild(laser);
    riesenLaser = laser;

    kamera.addShake(1.0f, 0.5f);
}

