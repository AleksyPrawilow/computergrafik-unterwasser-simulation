#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CRAB_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CRAB_H

#include "werkzeuge/wesen.h"

class Crab : public Wesen {
public:
    int type;
    Crab(int crabType = 1) : type(crabType) {}
    
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CRAB_H