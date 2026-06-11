//
// Created by Alexey Pravilov on 11/06/2026.
//

#include "unterwasserszeneWesen.h"

#include "dieWesen/earth.h"
#include "dieWesen/jellyfish.h"
#include "dieWesen/oceanFloor.h"
#include "dieWesen/wasser.h"
#include "dieWesen/ui/hudPanel.h"
#include "werkzeuge/ui/worldspaceUI.h"

void UnterwasserszeneWesen::init() {
    uboot = new Uboot();
    auto * jellyfish = new Jellyfish();
    addChild(uboot);
    addChild(new Earth());
    addChild(new OceanFloor());
    addChild(new Wasser());
    addChild(new HudPanel());
    addChild(jellyfish);

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
}
