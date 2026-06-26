//
// Created by Alexey Pravilov on 11/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEWESEN_H
#include "dieWesen/uboot.h"
#include "werkzeuge/wesen.h"


class UnterwasserszeneWesen : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
private:
    Uboot * uboot = nullptr;
    bool leviathanCutscene = false;
    bool cutsceneKameraAktiv = false;
    glm::vec3 cameraZielPos = glm::vec3(0.0f);
    glm::vec3 cameraBlickZiel = glm::vec3(0.0f);
    glm::vec3 leviathanEuler = glm::vec3(0.0f);
    Wesen* cutsceneLeviathan = nullptr;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEWESEN_H
