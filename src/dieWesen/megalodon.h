//
// Created by Alexey Pravilov on 25/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MEGALODON_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MEGALODON_H
#include "uboot.h"
#include "werkzeuge/wesen.h"


class Megalodon : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void prepareUniforms() const override;
    void getEaten();
    Wesen * followTarget = nullptr;
    bool isGrabbed = false;
private:
    bool triggeredCutscene = false;
    void cutscene();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MEGALODON_H
