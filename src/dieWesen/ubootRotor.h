//
// Created by Alexey Pravilov on 07/06/2026.
//

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UBOOTROTOR_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UBOOTROTOR_H
#include "werkzeuge/wesen.h"


class UbootRotor: public Wesen {
public:
    float rotorSpeed = 0.0f;
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UBOOTROTOR_H
