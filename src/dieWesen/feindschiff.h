#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FEINDSCHIFF_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FEINDSCHIFF_H

#include "../werkzeuge/wesen.h"
#include "timer.h"
#include <vector>

class Feindschiff : public Wesen {
public:
    std::vector<glm::vec3> wegpunkte;
    float leben = 1.0f;
    float laserSchaden = 10.0f;
    glm::vec3 laserGroesse = glm::vec3(0.05f, 0.05f, 2.5f);
    float laserOffset = 1.5f;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void schadenNehmen(float schaden);

    float bewegungsGeschwindigkeit = 5.0f;
    float drehGeschwindigkeit = 2.0f;
    float schussIntervall = 2.0f;

private:
    int aktuellerWegpunkt = 0;
    float schussTimer = 0.0f;

    void laserAbfeuern();
};

#endif
