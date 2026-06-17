//
// Created by Alexey Pravilov on 08/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TIMER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TIMER_H
#include <functional>
#include "werkzeuge/wesen.h"


class Timer: public Wesen {
public:
    bool active = false;

    void startTimer(float targetTime, std::function<void()> callbackFunc);
    void stopTimer();
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override {};
    [[nodiscard]] bool hasCustomRender() const override { return true;};
private:
    float currentTime{};
    float maxTime{};
    std::function<void()> callback {};
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TIMER_H
