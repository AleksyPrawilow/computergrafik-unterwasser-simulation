#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUSRUESTUNGSLEISTE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUSRUESTUNGSLEISTE_H

#include "werkzeuge/ui/wesenUI.h"
#include "werkzeuge/ui/uiContainers.h"
#include "werkzeuge/ui/uiLabel.h"
#include "inventarSlotUI.h"

class AusruestungsLeiste : public UIElement {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void aktualisieren();

private:
    HBoxUI* leisteContainer = nullptr;
    InventarSlotUI* slots[5] = {};
    UILabel* nummerLabels[5] = {};
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUSRUESTUNGSLEISTE_H
