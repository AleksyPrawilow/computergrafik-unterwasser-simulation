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
};

#endif
