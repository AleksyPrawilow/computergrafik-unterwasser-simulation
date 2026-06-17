#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ASTEROID_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ASTEROID_H

#include "../werkzeuge/wesen.h"

class Asteroid : public Wesen {
public:
    Asteroid(float orbitRadius = 0.0f, float orbitGeschwindigkeit = 0.0f, float drehGeschwindigkeit = 1.0f);
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    float orbitRadius;
    float orbitGeschwindigkeit;
    float drehGeschwindigkeit;
    glm::vec3 drehAchse;
    float zeitVersatz;
};

#endif
