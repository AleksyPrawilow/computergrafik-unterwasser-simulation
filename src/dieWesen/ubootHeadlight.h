//
// Created by Alexey Pravilov on 08/06/2026.
//

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UBOOTHEADLIGHT_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UBOOTHEADLIGHT_H
#include "timer.h"
#include "werkzeuge/lightManager.h"
#include "werkzeuge/wesen.h"


class UbootHeadlight : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
private:
    float currentPitch = 0.0f;
    SpotLight * spotlight {};
    Timer * timer {};
    void onTimerEnd();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UBOOTHEADLIGHT_H
