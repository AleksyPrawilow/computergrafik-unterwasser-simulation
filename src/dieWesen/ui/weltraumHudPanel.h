#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WELTRAUMHUDPANEL_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WELTRAUMHUDPANEL_H

#include "dieWesen/raumschiff.h"
#include "werkzeuge/ui/uiContainers.h"
#include "werkzeuge/ui/uiLabel.h"

class WeltraumHudPanel : public VBoxUI {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    Raumschiff * raumschiff = nullptr;
    UILabel * hpLabel = nullptr;
    UILabel * geschwindigkeitLabel = nullptr;
    UILabel * abschuessLabel = nullptr;
    UILabel * raketenLabel = nullptr;
    UILabel * feindeLabel = nullptr;

    UIElement * schadenVignette = nullptr;
    UIElement * bossBalkenHG = nullptr;
    UIElement * bossBalken = nullptr;
    UILabel * bossLabel = nullptr;

    static constexpr int KOMPASS_MAX = 25;
    static constexpr float KOMPASS_BREITE = 600.0f;
    UIElement* kompassHG = nullptr;
    UIElement* kompassPunkte[KOMPASS_MAX] = {};

    GLuint texCyan = 0;
    GLuint texRot = 0;
    GLuint texLila = 0;

    void kompassAktualisieren();
};

#endif
