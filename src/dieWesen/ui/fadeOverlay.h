//
// Created by Kajetan on 26/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FADEOVERLAY_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FADEOVERLAY_H
#include "werkzeuge/ui/wesenUI.h"
#include <functional>

class FadeOverlay : public UIElement {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;

    void fadeIn(float dauer, std::function<void()> callback = nullptr);
    void fadeOut(float dauer, std::function<void()> callback = nullptr);
    void sofort(float alpha);

private:
    float alpha = 0.0f;
    float zielAlpha = 0.0f;
    float geschwindigkeit = 0.0f;
    std::function<void()> fertigCallback = nullptr;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FADEOVERLAY_H
