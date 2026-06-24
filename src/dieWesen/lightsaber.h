//
// Created by Alexey Pravilov on 24/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LIGHTSABER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LIGHTSABER_H
#include "werkzeuge/wesen.h"


class Lightsaber : public Wesen {
public:
    explicit Lightsaber(const std::string& color);
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    glm::vec3 euler = glm::vec3(0.0f, 0.0f, 0.0f);
    int dir = 1;
private:
    std::string bladeColor = "default";
    Wesen * blade = nullptr;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LIGHTSABER_H
