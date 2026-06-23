//
// Created by Alexey Pravilov on 23/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MAP_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MAP_H
#include "werkzeuge/wesen.h"


class Map : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void prepareUniforms() const override;

    void unwrap(float duration = 1.4f);

    float unwrapProgress = 0.0f;
    glm::vec3 mapEuler;
private:
    bool isAnimating = false;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MAP_H
