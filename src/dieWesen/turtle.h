#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TURTLE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TURTLE_H

#include "werkzeuge/wesen.h"

class Turtle : public Wesen {
public:
    int type;
    Turtle(int turtleType = 1) : type(turtleType) {}
    
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TURTLE_H