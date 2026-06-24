//
// Created by Alexey Pravilov on 24/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CHEST_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CHEST_H
#include "player.h"
#include "werkzeuge/ui/uiLabel.h"

class Chest : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void open();
private:
    bool isDugOut = false;
    bool isOpened = false;
    Player * player = nullptr;
    Wesen * opening = nullptr;
    UILabel * interactLabel = nullptr;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CHEST_H
