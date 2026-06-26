//
// Created by Kajetan on 26/06/2026.
//

#include "raumschiffInnenWesen.h"

#include "dieWesen/player.h"
#include "dieWesen/feindschiff.h"
#include "dieWesen/aufhebbar.h"
#include "dieWesen/platzierbaresObjekt.h"
#include "dieWesen/ui/inventarHUD.h"
#include "dieWesen/ui/handwerkHUD.h"
#include "dieWesen/ui/ausruestungsLeiste.h"
#include "dieWesen/ui/schiffslaborHUD.h"
#include "werkzeuge/himmelsboxWesen.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/random.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/audio/musicManager.h"
#include "werkzeuge/visual/worldEnvironment.h"

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
    GLuint wandAlbedo = Kern::LoadTexture("assets/textures/raumschiff_albedo.png");
    GLuint wandNormal = Kern::LoadTexture("assets/textures/raumschiff_normal.png");
    GLuint wandRough = Kern::LoadTexture("assets/textures/raumschiff_roughness.png");

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
    for (float mx : {-22.0f, -11.0f, 0.0f, 11.0f, 22.0f}) {
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

    // --- Player (spawn in entry, facing north toward the hub) ---
    auto * player = new Player();
    player->affectedByWater = false;
    player->benutzeInsel = false;
    player->bodenHoehe = 0.0f;
    player->targetY = 3.0f;
    addChild(player);
    player->transform.position = glm::vec3(0.0f, 3.0f, 18.0f);

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

    // --- HUDs ---
    addChild(new InventarHUD());
    addChild(new HandwerkHUD());
    addChild(new SchiffslaborHUD());
    addChild(new AusruestungsLeiste());

    MusicManager::getInstance().playMusic("assets/audio/beatit.mp3", 1.5f, true);
}
