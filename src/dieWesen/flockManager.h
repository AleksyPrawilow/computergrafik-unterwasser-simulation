//
// Created by mikol on 22.06.2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FISHFLOCK_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FISHFLOCK_H

#include "werkzeuge/wesen.h"

class FishFlock : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    int numFishes = 30;
    float spawnRadius = 10.0f;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FISHFLOCK_H
