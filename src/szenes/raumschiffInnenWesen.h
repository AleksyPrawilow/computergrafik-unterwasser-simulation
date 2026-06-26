//
// Created by Kajetan on 26/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAUMSCHIFFINNENWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAUMSCHIFFINNENWESEN_H
#include "werkzeuge/wesen.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/ui/wesenUI.h"

class Player;

class RaumschiffInnenWesen : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    Player* spielerRef = nullptr;
    UILabel* lebenLabel = nullptr;
    UIElement* schadenVignette = nullptr;

    static constexpr int KOMPASS_MAX = 10;
    static constexpr float KOMPASS_BREITE = 500.0f;
    UIElement* kompassHG = nullptr;
    UIElement* kompassPunkte[KOMPASS_MAX] = {};

    void kompassAktualisieren();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAUMSCHIFFINNENWESEN_H
