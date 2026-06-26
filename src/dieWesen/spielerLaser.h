#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SPIELERLASER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SPIELERLASER_H

#include "werkzeuge/wesen.h"
#include "timer.h"

class SpielerLaser : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void abfeuern(const glm::vec3& position, const glm::quat& rotation);

private:
    float geschwindigkeit = 150.0f;
    float lebensdauer = 2.0f;
    bool istAbgefeuert = false;
    Timer * selbstzerstoerungsTimer = nullptr;
};

#endif
