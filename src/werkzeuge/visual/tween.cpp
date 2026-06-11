//
// Created by Alexey Pravilov on 08/06/2026.
//

#include "tween.h"

#include <iostream>

Tween* Tween::tweenInterval(float duration) {
    addProperty(std::make_unique<TweenDelay>(duration));
    return this;
}

Tween* Tween::tweenCallback(const std::function<void()>& callback) {
    addProperty(std::make_unique<TweenCallback>(callback));
    return this;
}

Tween* Tween::parallel() {
    parallelMode = true;
    return this;
}

Tween* Tween::setLoops(const int loopCount) {
    maxLoops = loopCount;
    return this;
}

bool Tween::update(const float deltaTime) {
    if (steps.empty()) return true;

    const auto& currentStep = steps[currentStepIndex];
    bool stepFinished = true;

    for (const auto& prop : currentStep) {
        if (!prop->update(deltaTime)) {
            stepFinished = false;
        }
    }

    if (stepFinished) {
        currentStepIndex++;
        if (currentStepIndex >= steps.size()) {
            currentLoop++;
            if (maxLoops == -1 || currentLoop < maxLoops) {
                currentStepIndex = 0;

                for (auto& step : steps) {
                    for (const auto& prop : step) {
                        prop->reset();
                    }
                }

                for (const auto& prop : steps[0]) {
                    prop->forceInitialStart();
                }

                return false;
            }
            return true;
        }
    }

    return false;
}

void Tween::addProperty(std::unique_ptr<TweenPropertyBase> prop) {
    if (parallelMode && !steps.empty()) {
        steps.back().push_back(std::move(prop));
    } else {
        std::vector<std::unique_ptr<TweenPropertyBase>> newStep;
        newStep.push_back(std::move(prop));
        steps.push_back(std::move(newStep));
    }
    parallelMode = false;
}

Tween* TweenManager::createTween() {
    auto tween = std::make_unique<Tween>();
    Tween* ptr = tween.get();
    activeTweens.push_back(std::move(tween));
    return ptr;
}

void TweenManager::update(const float deltaTime) {
    for (auto it = activeTweens.begin(); it != activeTweens.end();) {
        if ((*it)->update(deltaTime)) {
            it = activeTweens.erase(it);
        } else {
            ++it;
        }
    }
}

void TweenManager::cleanup() {
    activeTweens.clear();
}
