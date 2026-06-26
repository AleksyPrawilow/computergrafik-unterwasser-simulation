//
// Created by Alexey Pravilov on 23/06/2026.
//

#include "unterwasserszeneQuests.h"

#include "bottle.h"
#include "megalodon.h"
#include "ui/questCompletedBanner.h"
#include "werkzeuge/input.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/questManager.h"

UnterwasserszeneQuests::QUESTS UnterwasserszeneQuests::savedQuest = TUTORIAL;

void UnterwasserszeneQuests::init() {
    visible = false;
    name = "Quests";
    addToGroup("QuestManager");

    manager = &QuestManager::getInstance();
    audio = new AudioPlayer("assets/audio/quest_complete.wav", false, 2.0f, true, false);
    addChild(audio);
    setQuest(savedQuest);
}

void UnterwasserszeneQuests::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (Input::isKeyJustPressed(GLFW_KEY_J)) {
        QuestManager::getInstance().activeQuests[QuestManager::getInstance().activeQuests.size() - 1].isCompleted = true;
        QuestManager::getInstance().activeQuests[QuestManager::getInstance().activeQuests.size() - 1].onComplete();
    }

    switch (currentQuest) {
    case TUTORIAL:
        onUpdateTutorial();
        break;
    case COLLECT_BOTTLE:
        break;
    case THE_BEGINNING:
        break;
    case SURVIVAL:
        break;
    case DIG_OUT:
        break;
    case THROW_SUB:
        break;
    case EXPLORE_THE_SEA:
        if (cameraTransform.position.y < -5.0f) {
            manager->progressObjective("dive_below");
        }
        break;
    case FIND_THE_TRENCH:
        if (cameraTransform.position.y < -100.0f) {
            manager->progressObjective("reach_abyss");
        }
        break;
    case INVESTIGATE_THE_SOUNDS:
        break;
    }
}

void UnterwasserszeneQuests::setQuest(QUESTS newQuest) {
    currentQuest = newQuest;
    savedQuest = newQuest;
    switch (currentQuest) {
        case TUTORIAL:
            setupTutorial();
            break;
        case COLLECT_BOTTLE:
            setupCollectBottle();
            break;
        case THE_BEGINNING:
            setupTheBeginning();
            break;
        case SURVIVAL:
            setupSurvival();
            break;
        case DIG_OUT:
            setupDigOut();
            break;
        case THROW_SUB:
            setupThrowSub();
            break;
        case EXPLORE_THE_SEA:
            setupExploreTheSea();
            break;
        case FIND_THE_TRENCH:
            setupFindTheTrench();
            break;
        case INVESTIGATE_THE_SOUNDS:
            setupInvestigateTheSounds();
            break;
    }
}

void UnterwasserszeneQuests::setupTutorial() {
    Quest tutorialQuest;
    tutorialQuest.title = "Tutorial";

    QuestObjective wObjective;
    wObjective.tag = "tutorial_w";
    wObjective.description = "Press 'W' to go forward";
    wObjective.requiredCount = 1;

    QuestObjective sObjective;
    sObjective.tag = "tutorial_s";
    sObjective.description = "Press 'S' to go backwards";
    sObjective.requiredCount = 1;

    QuestObjective aObjective;
    aObjective.tag = "tutorial_a";
    aObjective.description = "Press 'A' to go to your left";
    aObjective.requiredCount = 1;

    QuestObjective dObjective;
    dObjective.tag = "tutorial_d";
    dObjective.description = "Press 'D' to go to your right";
    dObjective.requiredCount = 1;

    QuestObjective spaceObjective;
    spaceObjective.tag = "tutorial_space";
    spaceObjective.description = "Press 'SPACE' to jump";
    spaceObjective.requiredCount = 1;

    tutorialQuest.objectives.push_back(wObjective);
    tutorialQuest.objectives.push_back(sObjective);
    tutorialQuest.objectives.push_back(aObjective);
    tutorialQuest.objectives.push_back(dObjective);
    tutorialQuest.objectives.push_back(spaceObjective);

    tutorialQuest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("Tutorial completed!");
        parent->addChild(banner);
        audio->play();
        setQuest(COLLECT_BOTTLE);
    };

    QuestManager::getInstance().acceptQuest(tutorialQuest);
}

void UnterwasserszeneQuests::setupCollectBottle() {
    const auto& flaschen = getNodesInGroup("bottle");
    if (!flaschen.empty()) {
        auto* flasche = dynamic_cast<Bottle*>(flaschen[0]);
        if (flasche) flasche->aktiviereAbholung();
    }

    Quest collectBottleQuest;
    collectBottleQuest.title = "Mysterious Bottle";

    QuestObjective collectObjective;
    collectObjective.tag = "collect_bottle";
    collectObjective.description = "A mysterious bottle has drifted ashore. See what's inside it!";
    collectObjective.requiredCount = 1;

    collectBottleQuest.objectives.push_back(collectObjective);

    collectBottleQuest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("What is this map?");
        parent->addChild(banner);
        audio->play();

        setQuest(THE_BEGINNING);
    };

    QuestManager::getInstance().acceptQuest(collectBottleQuest);
}

void UnterwasserszeneQuests::setupTheBeginning() {
    Quest quest;
    quest.title = "The Beginning";

    QuestObjective axeObjective;
    axeObjective.tag = "craft_Axe";
    axeObjective.description = "Craft an axe";
    axeObjective.requiredCount = 1;

    QuestObjective workbenchObjective;
    workbenchObjective.tag = "craft_Workbench";
    workbenchObjective.description = "Craft a workbench";
    workbenchObjective.requiredCount = 1;

    QuestObjective furnaceObjective;
    furnaceObjective.tag = "craft_Furnace";
    furnaceObjective.description = "Craft a furnace";
    furnaceObjective.requiredCount = 1;

    quest.objectives.push_back(axeObjective);
    quest.objectives.push_back(workbenchObjective);
    quest.objectives.push_back(furnaceObjective);

    quest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("Now you can build!");
        parent->addChild(banner);
        audio->play();
        setQuest(SURVIVAL);
    };

    QuestManager::getInstance().acceptQuest(quest);
}

void UnterwasserszeneQuests::setupDigOut() {
    Quest digOutQuest;
    digOutQuest.title = "Treasure";

    QuestObjective craftObjective;
    craftObjective.tag = "craft_Shovel";
    craftObjective.description = "Craft a shovel at the workbench";
    craftObjective.requiredCount = 1;

    QuestObjective digObjective;
    digObjective.tag = "dig_out";
    digObjective.description = "Dig out the treasure";
    digObjective.requiredCount = 1;

    digOutQuest.objectives.push_back(craftObjective);
    digOutQuest.objectives.push_back(digObjective);

    digOutQuest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("The treasure is yours!");
        parent->addChild(banner);
        audio->play();
        setQuest(THROW_SUB);
    };

    QuestManager::getInstance().acceptQuest(digOutQuest);
}

void UnterwasserszeneQuests::setupSurvival() {
    Quest quest;
    quest.title = "Survival";

    QuestObjective raftObjective;
    raftObjective.tag = "craft_Raft";
    raftObjective.description = "Craft a raft";
    raftObjective.requiredCount = 1;

    QuestObjective rodObjective;
    rodObjective.tag = "craft_Fishing Rod";
    rodObjective.description = "Craft a fishing rod";
    rodObjective.requiredCount = 1;

    QuestObjective fishObjective;
    fishObjective.tag = "catch_fish";
    fishObjective.description = "Catch something from the sea";
    fishObjective.requiredCount = 1;

    QuestObjective houseObjective;
    houseObjective.tag = "craft_House";
    houseObjective.description = "Build a house";
    houseObjective.requiredCount = 1;

    quest.objectives.push_back(raftObjective);
    quest.objectives.push_back(rodObjective);
    quest.objectives.push_back(fishObjective);
    quest.objectives.push_back(houseObjective);

    quest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("Time to find the treasure!");
        parent->addChild(banner);
        audio->play();
        setQuest(DIG_OUT);
    };

    QuestManager::getInstance().acceptQuest(quest);
}

void UnterwasserszeneQuests::setupThrowSub() {
    Quest submarineQuest;
    submarineQuest.title = "Submarine";

    QuestObjective collectObjective;
    collectObjective.tag = "collect_Mini U-Boot";
    collectObjective.description = "Pick up the submarine";
    collectObjective.requiredCount = 1;

    QuestObjective throwObjective;
    throwObjective.tag = "submarine_throw";
    throwObjective.description = "It is quite small... Maybe it needs water?";
    throwObjective.requiredCount = 1;

    submarineQuest.objectives.push_back(collectObjective);
    submarineQuest.objectives.push_back(throwObjective);

    submarineQuest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("Let the adventure begin!");
        parent->addChild(banner);
        audio->play();
        setQuest(EXPLORE_THE_SEA);
    };

    QuestManager::getInstance().acceptQuest(submarineQuest);
}

void UnterwasserszeneQuests::setupExploreTheSea() {
    Quest quest;
    quest.title = "Into the Deep";

    QuestObjective diveObjective;
    diveObjective.tag = "dive_below";
    diveObjective.description = "Dive below the surface";
    diveObjective.requiredCount = 1;

    quest.objectives.push_back(diveObjective);

    quest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("The ocean awaits...");
        parent->addChild(banner);
        audio->play();
        setQuest(FIND_THE_TRENCH);
    };

    QuestManager::getInstance().acceptQuest(quest);
}

void UnterwasserszeneQuests::setupFindTheTrench() {
    Quest quest;
    quest.title = "The Abyss";

    QuestObjective depthObjective;
    depthObjective.tag = "reach_abyss";
    depthObjective.description = "Descend into the abyss";
    depthObjective.requiredCount = 1;

    quest.objectives.push_back(depthObjective);

    quest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("What lurks in the darkness?");
        parent->addChild(banner);
        audio->play();
        setQuest(INVESTIGATE_THE_SOUNDS);
    };

    QuestManager::getInstance().acceptQuest(quest);
}

void UnterwasserszeneQuests::setupInvestigateTheSounds() {
    Quest quest;
    quest.title = "The bigger fish";

    QuestObjective encounterObjective;
    encounterObjective.tag = "leviathan_encounter";
    encounterObjective.description = "Investigate the strange sounds";
    encounterObjective.requiredCount = 1;

    quest.objectives.push_back(encounterObjective);

    quest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("You survived!");
        parent->addChild(banner);
        audio->play();
    };

    QuestManager::getInstance().acceptQuest(quest);
}

void UnterwasserszeneQuests::onUpdateTutorial() {
    if (Input::isKeyJustPressed(GLFW_KEY_W)) {
        manager->progressObjective("tutorial_w", 1);
    }

    if (Input::isKeyJustPressed(GLFW_KEY_S)) {
        manager->progressObjective("tutorial_s", 1);
    }

    if (Input::isKeyJustPressed(GLFW_KEY_A)) {
        manager->progressObjective("tutorial_a", 1);
    }

    if (Input::isKeyJustPressed(GLFW_KEY_D)) {
        manager->progressObjective("tutorial_d", 1);
    }

    if (Input::isKeyJustPressed(GLFW_KEY_SPACE)) {
        manager->progressObjective("tutorial_space", 1);
    }
}
