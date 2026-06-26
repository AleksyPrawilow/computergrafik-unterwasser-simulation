//
// Created by Alexey Pravilov on 26/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_STARWARSINTRO_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_STARWARSINTRO_H
#include "werkzeuge/wesen.h"

class StarWarsIntro : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void prepareUniforms() const override;
};



#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_STARWARSINTRO_H
