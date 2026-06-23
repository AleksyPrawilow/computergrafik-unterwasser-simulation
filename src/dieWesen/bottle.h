//
// Created by Alexey Pravilov on 20/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_BOTTLE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_BOTTLE_H
#include "map.h"
#include "player.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/ui/worldspaceUI.h"


class Bottle : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void beginSwim();
    void pickup();
private:
    UILabel * interactLabel = nullptr;
    WorldspaceUI * uiMarker = nullptr;
    Wesen * cork = nullptr;
    Player * player = nullptr;
    Map * map = nullptr;
    glm::vec3 euler = glm::vec3(0.0f, 0.0f, 0.0f);
    float tiltStrength = 0.35f;
    float heightSmooth = 8.0f;
    float rotSmooth = 6.0f;
    bool canBePicked = false;
    bool isPicked = false;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_BOTTLE_H
