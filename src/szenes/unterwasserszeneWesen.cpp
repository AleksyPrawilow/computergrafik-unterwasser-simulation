//
// Created by Alexey Pravilov on 11/06/2026.
//

#include "unterwasserszeneWesen.h"

#include "werkzeuge/himmelsboxWesen.h"
#include "dieWesen/earth.h"
#include "dieWesen/island.h"
#include "dieWesen/jellyfish.h"
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
#include "dieWesen/rock.h"

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
    addChild(uboot);
    addChild(new Earth());
    addChild(new OceanFloor());
    addChild(new Wasser());
    addChild(new HudPanel());
    addChild(new Island());
    addChild(new Player());
    addChild(jellyfish);

    auto * worldEnv = new WorldEnvironment();
    worldEnv->init();

    worldEnv->params.fogColor = glm::vec3(0.0f, 0.05f, 0.15f);

    // Bright surface/ceiling color
    worldEnv->params.heightFogColor = glm::vec3(0.0f, 0.22f, 0.28f);

    worldEnv->params.fogDensity = 0.025f;

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

    AudioManager::getInstance().play2D("assets/audio/abyss.mp3", true, true);
}
