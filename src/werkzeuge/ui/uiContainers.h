//
// Created by Alexey Pravilov on 10/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UICONTAINERS_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UICONTAINERS_H
#include "wesenUI.h"


class VBoxUI : public UIElement {
public:
    float spacing = 8.0f;

    void setSpacing(float newSpacing = 8.0f);
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};

class HBoxUI : public UIElement {
public:
    float spacing = 8.0f;

    void setSpacing(float newSpacing = 8.0f);
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UICONTAINERS_H
