//
// Created by Alexey Pravilov on 10/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HUDPANEL_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HUDPANEL_H
#include "dieWesen/uboot.h"
#include "werkzeuge/ui/uiContainers.h"
#include "werkzeuge/ui/uiLabel.h"


class HudPanel : public VBoxUI {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void setHealth(float newHealth) const;
    void setDepth(float newDepth) const;
    void setSpeed(float newSpeed) const;
    Uboot * uboot = nullptr;
private:
    UILabel * hpLabel = nullptr;
    UILabel * depthLabel = nullptr;
    UILabel * speedLabel = nullptr;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HUDPANEL_H
