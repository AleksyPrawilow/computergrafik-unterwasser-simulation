//
// Created by Alexey Pravilov on 20/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTHUD_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTHUD_H
#include "werkzeuge/ui/uiContainers.h"


class UILabel;

class QuestHUD : public VBoxUI {
public:
    explicit QuestHUD(float spacing = 6.0f);
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void refreshUI();
private:
    std::vector<UILabel*> labelPool;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTHUD_H
