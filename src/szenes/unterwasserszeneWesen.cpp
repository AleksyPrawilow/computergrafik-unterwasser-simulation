//
// Created by Alexey Pravilov on 11/06/2026.
//

#include "unterwasserszeneWesen.h"

#include "werkzeuge/himmelsboxWesen.h"
#include "dieWesen/earth.h"
#include "dieWesen/island.h"
#include "dieWesen/jellyfish.h"
#include "dieWesen/flockManager.h"
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
#include "werkzeuge/audio/musicManager.h"
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

    uboot = new Uboot();
    auto * jellyfish = new Jellyfish();
    FishFlock* flock = new FishFlock();
    addChild(uboot);
    addChild(new Earth());
    addChild(new OceanFloor());
    addChild(new Wasser());
    addChild(new HudPanel());
    addChild(new Island());
    addChild(new Player());
    addChild(jellyfish);
    flock->transform.position = glm::vec3(-12.f, -10.f, 0.f);
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
    addChild(worldEnv);

    // TEST
    auto * worldspaceUI = new WorldspaceUI();
    addChild(worldspaceUI);
    worldspaceUI->shouldScale = true;;
    worldspaceUI->setTarget(jellyfish);

    auto * label = new UILabel();
    label->setExpansion(UIExpansion::CENTER);
    label->setText("Hello world!", 64.0f);
    worldspaceUI->addChild(label);
    // TEST

    auto* questHUD = new QuestHUD();
    addChild(questHUD);

    Quest chopTreeQuest;
    chopTreeQuest.title = "Wood";

    QuestObjective digObjective;
    digObjective.tag = "chop_tree";
    digObjective.description = "Chop down the tree";
    digObjective.requiredCount = 1;

    chopTreeQuest.objectives.push_back(digObjective);

    chopTreeQuest.onComplete = [this]() {
        std::cout << "QUEST COMPLETED: You found the sunken treasure!" << std::endl;
        auto* banner = new QuestCompletedBanner("The tree is no more");
        this->addChild(banner);
        AudioManager::getInstance().play2D("assets/audio/quest_complete.wav", false, true);
    };

    QuestManager::getInstance().acceptQuest(chopTreeQuest);

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

    addChild(new Thunderstorm());

    addChild(new UnterwasserszeneAudioHelper());
}
