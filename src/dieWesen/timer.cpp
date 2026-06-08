//
// Created by Alexey Pravilov on 08/06/2026.
//

#include "timer.h"

#include <utility>

void Timer::startTimer(const float targetTime, std::function<void()> callbackFunc) {
    active = true;
    currentTime = 0.0f;
    maxTime = targetTime;
    callback = std::move(callbackFunc);
}

void Timer::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!active) {
        return;
    }

    currentTime += deltaTime;
    if (currentTime >= maxTime) {
        active = false;
        if (callback != nullptr) {
            callback();
        }
    }
}
