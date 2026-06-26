//
// Created by Alexey Pravilov on 26/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SPACESHIPENTERABLE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SPACESHIPENTERABLE_H
#include "player.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/ui/uiLabel.h"

class SpaceshipEnterable : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
private:
    Player * player = nullptr;
    UILabel * interactLabel = nullptr;
    bool cutsceneTriggered = false;
    glm::quat targetCameraRotation = glm::quat();
    glm::vec3 euler = glm::vec3(0.0f, 120.0f, 0.0f);

    void cutscene();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SPACESHIPENTERABLE_H
