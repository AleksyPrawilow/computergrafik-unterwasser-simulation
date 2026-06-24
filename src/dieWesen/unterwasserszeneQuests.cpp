//
// Created by Alexey Pravilov on 23/06/2026.
//

#include "unterwasserszeneQuests.h"

#include "bottle.h"
#include "ui/questCompletedBanner.h"
#include "werkzeuge/input.h"
#include "werkzeuge/visual/questManager.h"

void UnterwasserszeneQuests::init() {
    visible = false;
    name = "Quests";
    addToGroup("QuestManager");

    manager = &QuestManager::getInstance();
    audio = new AudioPlayer("assets/audio/quest_complete.wav", false, 2.0f, true, false);
    addChild(audio);
    setQuest(TUTORIAL);
}

void UnterwasserszeneQuests::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    switch (currentQuest) {
    case TUTORIAL:
        onUpdateTutorial();
        break;
    case COLLECT_BOTTLE:
        break;
    case CHOP_DOWN:
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
    switch (currentQuest) {
        case TUTORIAL:
            setupTutorial();
            break;
        case COLLECT_BOTTLE:
            setupCollectBottle();
            break;
        case CHOP_DOWN:
            setupChopDown();
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

        setQuest(DIG_OUT);
    };

    QuestManager::getInstance().acceptQuest(collectBottleQuest);
}

void UnterwasserszeneQuests::setupChopDown() {
    Quest chopTreeQuest;
    chopTreeQuest.title = "Wood";

    chopTreeQuest.onComplete = [this]() {
        auto * banner = new QuestCompletedBanner("The tree is no more");
        parent->addChild(banner);
        audio->play();

        setQuest(DIG_OUT);
    };

    QuestManager::getInstance().acceptQuest(chopTreeQuest);
}

void UnterwasserszeneQuests::setupDigOut() {
    Quest digOutQuest;
    digOutQuest.title = "Treasure";

    QuestObjective chopObjective;
    chopObjective.tag = "chop_tree";
    chopObjective.description = "Chop down the tree for wood";
    chopObjective.requiredCount = 1;

    QuestObjective craftObjective;
    craftObjective.tag = "craft_Shovel";
    craftObjective.description = "Craft a shovel";
    craftObjective.requiredCount = 1;

    QuestObjective digObjective;
    digObjective.tag = "dig_out";
    digObjective.description = "Dig out the treasure";
    digObjective.requiredCount = 1;

    digOutQuest.objectives.push_back(chopObjective);
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
    quest.title = "The Leviathan";

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
