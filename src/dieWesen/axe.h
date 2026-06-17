//
// Created by Alexey Pravilov on 17/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AXE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AXE_H
#include "tree.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/audio/audioPlayer.h"


class Timer;

class Axe: public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
private:
    bool isPlayingAnimation = false;
    bool recoveringAnimation = false;
    glm::vec3 weaponEuler;
    glm::vec3 hitNormal;
    Tree * treeToHit = nullptr;
    Timer * recoveryTimer = nullptr;
    AudioPlayer * hitSound = nullptr;
    AudioPlayer * swingSound = nullptr;

    void swing();
    void recoverAnimation();
    void hitTree();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AXE_H
