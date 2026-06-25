#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SCHILDPICKUP_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SCHILDPICKUP_H

#include "werkzeuge/wesen.h"

class SchildPickup : public Wesen {
public:
    explicit SchildPickup(const glm::vec3& position);
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    glm::vec3 startPosition;
    float alter = 0.0f;
    float lebensdauer = 10.0f;
};

#endif
