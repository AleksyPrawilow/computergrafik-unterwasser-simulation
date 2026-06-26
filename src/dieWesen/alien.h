#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ALIEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ALIEN_H

#include "werkzeuge/wesen.h"
#include <vector>

class Alien : public Wesen {
public:
    float leben = 3.0f;
    std::vector<glm::vec3> wegpunkte;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void schadenNehmen(float schaden);

private:
    int aktuellerWegpunkt = 0;
    float schussTimer = 0.0f;
    float schussIntervall = 2.5f;
    float geschwindigkeit = 3.0f;
    float bodenY = 1.5f;

    void laserAbfeuern();
    bool hatSichtlinie(const glm::vec3& von, const glm::vec3& nach) const;
};

#endif
