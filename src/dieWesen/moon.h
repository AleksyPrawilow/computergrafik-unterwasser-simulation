//
// Created by Alexey Pravilov on 05/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MOON_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MOON_H
#include "werkzeuge/wesen.h"


class Moon: public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MOON_H
