#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ALIENLASER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ALIENLASER_H

#include "werkzeuge/wesen.h"
#include "timer.h"

class AlienLaser : public Wesen {
public:
    float schaden = 15.0f;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void abfeuern(const glm::vec3& position, const glm::quat& rotation);

private:
    float geschwindigkeit = 80.0f;
    float lebensdauer = 3.0f;
    bool istAbgefeuert = false;
    Timer * selbstzerstoerungsTimer = nullptr;
};

#endif
