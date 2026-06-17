//
// Created by Alexey Pravilov on 17/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAFT_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAFT_H
#include "werkzeuge/wesen.h"


class Raft : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
private:
    glm::vec2 halfSize = {2.0f, 4.0f};
    float tiltStrength = 0.15f;
    float heightSmooth = 8.0f;
    float rotSmooth = 6.0f;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAFT_H
