//
// Created by Alexey Pravilov on 21/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEAUDIOHELPER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEAUDIOHELPER_H
#include "werkzeuge/wesen.h"
#include "werkzeuge/audio/musicManager.h"

class UnterwasserszeneAudioHelper : public Wesen {
public:
    enum GameState {
        OVER_WATER,
        UNDER_WATER,
        ABYSS,
        DEEP_ABYSS,
        ESCAPE,
        ALIENS
    };
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    [[nodiscard]] bool hasCustomRender() const override { return true; };

    void setGameState(GameState state);
private:
    MusicManager * musicManager = nullptr;
    GameState gameState = OVER_WATER;

    void manageIsland(const Transform& cameraTransform);
    void manageUnderwater(const Transform& cameraTransform);
    void manageAbyss(const Transform& cameraTransform);
    void manageDeepAbyss(const Transform& cameraTransform);
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEAUDIOHELPER_H
