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
        break;
    case FIND_THE_TRENCH:
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
    };

    QuestManager::getInstance().acceptQuest(digOutQuest);
}

void UnterwasserszeneQuests::setupThrowSub() {

}

void UnterwasserszeneQuests::setupExploreTheSea() {

}

void UnterwasserszeneQuests::setupFindTheTrench() {

}

void UnterwasserszeneQuests::setupInvestigateTheSounds() {

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
