#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FEINDSCHIFF_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FEINDSCHIFF_H

#include "../werkzeuge/wesen.h"
#include "timer.h"
#include <vector>

class Feindschiff : public Wesen {
public:
    std::vector<glm::vec3> wegpunkte;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    int aktuellerWegpunkt = 0;
    float bewegungsGeschwindigkeit = 5.0f;
    float drehGeschwindigkeit = 2.0f;
    float schussIntervall = 2.0f;
    float schussTimer = 0.0f;

    void laserAbfeuern();
};

#endif
