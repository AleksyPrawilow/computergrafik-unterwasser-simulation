//
// Created by Alexey Pravilov on 17/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TREE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TREE_H
#include "werkzeuge/wesen.h"
#include "werkzeuge/audio/audioPlayer.h"


class Tree : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void hit(glm::vec3 hitDir);
    void fall(glm::vec3 hitDir);
private:
    AudioPlayer * fallSound = nullptr;
    glm::vec3 treeEuler;
    int hp = 10;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TREE_H
