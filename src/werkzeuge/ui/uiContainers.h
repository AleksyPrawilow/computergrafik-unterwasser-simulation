//
// Created by Alexey Pravilov on 10/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UICONTAINERS_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UICONTAINERS_H
#include "wesenUI.h"


enum class UIAlignment {
    START,
    CENTER,
    END
};

class VBoxUI : public UIElement {
public:
    float spacing = 8.0f;
    UIAlignment alignment = UIAlignment::START;

    explicit VBoxUI(const float spacing = 8.0f) : spacing(spacing) {}

    VBoxUI* setAlignment(UIAlignment align);

    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};

class HBoxUI : public UIElement {
public:
    float spacing = 8.0f;
    UIAlignment alignment = UIAlignment::CENTER;

    explicit HBoxUI(const float spacing = 8.0f) : spacing(spacing) {}

    HBoxUI* setAlignment(UIAlignment align);

    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UICONTAINERS_H
