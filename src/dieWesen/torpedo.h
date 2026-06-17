#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TORPEDO_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TORPEDO_H

#include "../werkzeuge/wesen.h"
#include "timer.h"

class Torpedo : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void abfeuern(const glm::vec3& position, const glm::quat& rotation);

private:
    float geschwindigkeit = 50.0f;
    float lebensdauer = 3.0f;
    bool istAbgefeuert = false;
    Timer * selbstzerstoerungsTimer = nullptr;

    void kollisionPruefen();
};

#endif
