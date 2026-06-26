//
// Created by Alexey Pravilov on 23/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEQUESTS_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEQUESTS_H
#include "werkzeuge/wesen.h"
#include "werkzeuge/audio/audioPlayer.h"
#include "werkzeuge/ui/worldspaceUI.h"
#include "werkzeuge/visual/questManager.h"


class UnterwasserszeneQuests : public Wesen {
public:
    enum QUESTS {
        TUTORIAL,
        COLLECT_BOTTLE,
        THE_BEGINNING,
        SURVIVAL,
        DIG_OUT,
        THROW_SUB,
        EXPLORE_THE_SEA,
        FIND_THE_TRENCH,
        INVESTIGATE_THE_SOUNDS
    };

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void setQuest(QUESTS newQuest);

    QUESTS currentQuest = TUTORIAL;
    static QUESTS savedQuest;
    WorldspaceUI * uiMarker = nullptr;
private:
    QuestManager * manager = nullptr;
    AudioPlayer * audio = nullptr;

    void setupTutorial();
    void setupCollectBottle();
    void setupTheBeginning();
    void setupSurvival();
    void setupDigOut();
    void setupThrowSub();
    void setupExploreTheSea();
    void setupFindTheTrench();
    void setupInvestigateTheSounds();

    void onUpdateTutorial();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEQUESTS_H
