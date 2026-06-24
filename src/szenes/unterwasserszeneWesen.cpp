//
// Created by Alexey Pravilov on 11/06/2026.
//

#include "unterwasserszeneWesen.h"

#include "werkzeuge/himmelsboxWesen.h"
#include "dieWesen/earth.h"
#include "dieWesen/island.h"
#include "dieWesen/jellyfish.h"
#include "dieWesen/flockManager.h"
#include "dieWesen/path3D.h"
#include "dieWesen/pathFollower.h"
#include "dieWesen/oceanFloor.h"
#include "dieWesen/player.h"
#include "dieWesen/raft.h"
#include "dieWesen/tree.h"
#include "dieWesen/wasser.h"
#include "dieWesen/ui/hudPanel.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/ui/worldspaceUI.h"
#include "werkzeuge/visual/worldEnvironment.h"
#include "unterwasserszeneProps.h"
#include "dieWesen/bottle.h"
#include "dieWesen/rock.h"
#include "dieWesen/thunderstorm.h"
#include "dieWesen/unterwasserszeneAudioHelper.h"
#include "dieWesen/ui/questCompletedBanner.h"
#include "dieWesen/ui/questHUD.h"
#include "dieWesen/ui/inventarHUD.h"
#include "dieWesen/ui/handwerkHUD.h"
#include "dieWesen/ui/werkbankHUD.h"
#include "dieWesen/ui/ausruestungsLeiste.h"
#include "dieWesen/aufhebbar.h"
#include "dieWesen/chest.h"
#include "dieWesen/leviathan.h"
#include "dieWesen/unterwasserszeneQuests.h"
#include "werkzeuge/audio/musicManager.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/visual/questManager.h"

void UnterwasserszeneWesen::init() {
    addChild(new HimmelsboxWesen({
        "assets/textures/skybox/px.png",
        "assets/textures/skybox/nx.png",
        "assets/textures/skybox/py.png",
        "assets/textures/skybox/ny.png",
        "assets/textures/skybox/pz.png",
        "assets/textures/skybox/nz.png"
    }));

    addChild(new UnterwasserszeneAudioHelper());
    auto * jellyfish = new Jellyfish();
    FishFlock* flock = new FishFlock();
    Path3D* fishPath = new Path3D();
    addChild(new Earth());
    addChild(new OceanFloor());
    addChild(new Wasser());
    addChild(new Island());
    addChild(new Player());
    addChild(new Leviathan());
    addChild(jellyfish);

    // fishPath->transform.position = glm::vec3(-700.0f, -10.0f, -210.0f);
    fishPath->transform.position = glm::vec3(-12.f, -10.f, 0.f);
    fishPath->generateFigureEight(40.0f, 10.0f, 40.0f);
    addChild(fishPath);
    flock->transform.position = fishPath->transform.position;
    flock->targetPath = fishPath;
    // flock->transform.position = glm::vec3(-12.f, -10.f, 0.f);
    addChild(flock);

    auto * bottle = new Bottle();
    bottle->transform.position = glm::vec3(-700, 20.0f, -210.0f);
    addChild(bottle);

    auto * worldEnv = new WorldEnvironment();
    worldEnv->init();
    worldEnv->params.sunEnergy = 4.0f;
    worldEnv->params.sunDirection = glm::vec3(0.004f, 0.055f, 0.998f);
    worldEnv->params.fogColor = glm::vec3(0.0f, 0.05f, 0.15f);
    worldEnv->params.heightFogColor = glm::vec3(0.0f, 0.22f, 0.28f);
    worldEnv->params.fogDensity = 0.025f;
    worldEnv->params.depthDimmingCoefficient = 0.01;
    worldEnv->params.bloomThreshold = 0.0f;
    worldEnv->params.bloomIntensity = 3.0f;
    addChild(worldEnv);

    auto* questHUD = new QuestHUD();
    addChild(questHUD);

    Inventar::getInstance().hinzufuegen(GegenstandID::HOLZAXT, 1);
    Inventar::getInstance().hinzufuegen(GegenstandID::MINIUBOOT, 1);

    addChild(new InventarHUD());
    addChild(new HandwerkHUD());
    addChild(new WerkbankHUD());
    addChild(new AusruestungsLeiste());

    auto* seilPickup = new Aufhebbar(GegenstandID::SEIL, 2);
    seilPickup->transform.position = glm::vec3(-695.0f, 14.0f, -215.0f);
    addChild(seilPickup);

    auto* steinPickup = new Aufhebbar(GegenstandID::STEIN, 3);
    steinPickup->transform.position = glm::vec3(-705.0f, 14.0f, -225.0f);
    addChild(steinPickup);

    auto* flaschePickup = new Aufhebbar(GegenstandID::FLASCHE, 1);
    flaschePickup->transform.position = glm::vec3(-700.0f, 20.0f, -210.0f);
    addChild(flaschePickup);

    auto * chest = new Chest();
    chest->transform.position = glm::vec3(-690.0f, 11.5f, -225.0f);
    addChild(chest);

    auto sceneData = getGodotSceneData();
    for (const auto& [className, transforms] : sceneData) {
        for (const auto& t : transforms) {
            Wesen * entity = nullptr;
            if (className == "rock") {
                entity = new Rock(t.position, t.rotation, t.scale);
            }
            if (entity != nullptr) {
                addChild(entity);
            }
        }
    }

    addChild(new UnterwasserszeneQuests());
    addChild(new Thunderstorm());
}
