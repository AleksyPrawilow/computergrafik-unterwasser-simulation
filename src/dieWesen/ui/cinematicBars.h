//
// Created by Alexey Pravilov on 26/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CINEMATICBARS_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CINEMATICBARS_H
#include "werkzeuge/ui/wesenUI.h"

class CinematicBars : public UIElement {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;

    void setEnabled(bool enabled);
    [[nodiscard]] bool isEnabled() const { return targetProgress > 0.5f; }

private:
    float progress = 0.0f;
    float targetProgress = 0.0f;
    float transitionSpeed = 4.0f;
    float heightPercentage = 0.12f;
};
#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CINEMATICBARS_H
