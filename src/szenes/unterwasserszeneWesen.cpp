//
// Created by Alexey Pravilov on 11/06/2026.
//

#include "unterwasserszeneWesen.h"

#include "dieWesen/earth.h"
#include "dieWesen/jellyfish.h"
#include "dieWesen/oceanFloor.h"
#include "dieWesen/wasser.h"
#include "dieWesen/ui/hudPanel.h"
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/ui/worldspaceUI.h"
#include "werkzeuge/visual/worldEnvironment.h"

void UnterwasserszeneWesen::init() {
    uboot = new Uboot();
    auto * jellyfish = new Jellyfish();
    addChild(uboot);
    addChild(new Earth());
    addChild(new OceanFloor());
    addChild(new Wasser());
    addChild(new HudPanel());
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

    AudioManager::getInstance().play2D("assets/audio/abyss.mp3", true, true);
}
