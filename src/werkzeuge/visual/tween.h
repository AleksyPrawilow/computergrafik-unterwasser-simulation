//
// Created by Alexey Pravilov on 08/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TWEEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TWEEN_H

#include <functional>
#include <glm.hpp>
#include <memory>
#include <vector>

class Wesen;

enum class EaseType {
    LINEAR,
    EASE_IN,            // Quad In
    EASE_OUT,           // Quad Out
    EASE_IN_OUT,        // Quad In Out

    EASE_IN_SINE,       // Softest organic acceleration
    EASE_OUT_SINE,      // Softest organic deceleration
    EASE_IN_OUT_SINE,   // Softest organic acceleration and deceleration

    EASE_IN_CUBIC,      // Stronger acceleration
    EASE_OUT_CUBIC,     // Stronger deceleration
    EASE_IN_OUT_CUBIC,  // Stronger acceleration and deceleration

    EASE_IN_CIRC,       // Slow start, extremely sudden acceleration
    EASE_OUT_CIRC,      // Extremely fast start, slow deceleration
    EASE_IN_OUT_CIRC,   // Sudden mid-animation acceleration

    EASE_OUT_BACK,
    EASE_OUT_BOUNCE,
    EASE_OUT_ELASTIC
};

inline float ApplyComplexEasing(float t, const EaseType ease) {
    constexpr float PI = 3.14159265359f;

    if (ease == EaseType::LINEAR) return t;
    if (ease == EaseType::EASE_IN) return t * t;
    if (ease == EaseType::EASE_OUT) return t * (2.0f - t);
    if (ease == EaseType::EASE_IN_OUT) return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;

    if (ease == EaseType::EASE_IN_SINE) {
        return 1.0f - glm::cos((t * PI) / 2.0f);
    }
    if (ease == EaseType::EASE_OUT_SINE) {
        return glm::sin((t * PI) / 2.0f);
    }
    if (ease == EaseType::EASE_IN_OUT_SINE) {
        return -(glm::cos(PI * t) - 1.0f) / 2.0f;
    }

    if (ease == EaseType::EASE_IN_CUBIC) {
        return t * t * t;
    }
    if (ease == EaseType::EASE_OUT_CUBIC) {
        const float t1 = t - 1.0f;
        return 1.0f + t1 * t1 * t1;
    }
    if (ease == EaseType::EASE_IN_OUT_CUBIC) {
        const float t1 = -2.0f * t + 2.0f;
        return t < 0.5f ? 4.0f * t * t * t : 1.0f - (t1 * t1 * t1) / 2.0f;
    }

    if (ease == EaseType::EASE_IN_CIRC) {
        return 1.0f - glm::sqrt(1.0f - t * t);
    }
    if (ease == EaseType::EASE_OUT_CIRC) {
        const float t1 = t - 1.0f;
        return glm::sqrt(1.0f - t1 * t1);
    }
    if (ease == EaseType::EASE_IN_OUT_CIRC) {
        const float t1 = 2.0f * t;
        const float t2 = -2.0f * t + 2.0f;
        return t < 0.5f
            ? (1.0f - glm::sqrt(1.0f - t1 * t1)) / 2.0f
            : (glm::sqrt(1.0f - t2 * t2) + 1.0f) / 2.0f;
    }

    if (ease == EaseType::EASE_OUT_BACK) {
        constexpr float c1 = 1.70158f;
        constexpr float c3 = c1 + 1.0f;
        const float t1 = t - 1.0f;
        return 1.0f + c3 * t1 * t1 * t1 + c1 * t1 * t1;
    }

    if (ease == EaseType::EASE_OUT_BOUNCE) {
        constexpr float n1 = 7.5625f;
        constexpr float d1 = 2.75f;
        if (t < 1.0f / d1) {
            return n1 * t * t;
        } else if (t < 2.0f / d1) {
            t -= 1.5f / d1;
            return n1 * t * t + 0.75f;
        } else if (t < 2.5f / d1) {
            t -= 2.25f / d1;
            return n1 * t * t + 0.9375f;
        } else {
            t -= 2.625f / d1;
            return n1 * t * t + 0.984375f;
        }
    }

    if (ease == EaseType::EASE_OUT_ELASTIC) {
        constexpr float c4 = (2.0f * 3.14159265f) / 3.0f;
        if (t == 0.0f) return 0.0f;
        if (t == 1.0f) return 1.0f;
        return glm::pow(2.0f, -10.0f * t) * glm::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
    }

    return t;
}

class TweenPropertyBase {
public:
    virtual ~TweenPropertyBase() = default;
    virtual bool update(float deltaTime) = 0;
    virtual void reset() = 0;
    virtual void forceInitialStart() = 0;
};

template<typename T>
class TweenProperty : public TweenPropertyBase {
public:
    T* target;
    T start;
    T initialStart;
    T end;
    float duration;
    float elapsed = 0.0f;
    EaseType ease;
    bool isStarted = false;

    TweenProperty(T* target, const T& endVal, const float duration, const EaseType ease)
        : target(target), start(*target), end(endVal), duration(duration), ease(ease) {}

    bool update(const float deltaTime) override {
        if (target == nullptr) return true;

        if (!isStarted) {
            start = *target;
            isStarted = true;
        }

        elapsed += deltaTime;
        const float t = glm::clamp(elapsed / duration, 0.0f, 1.0f);
        float easedT = ApplyComplexEasing(t, ease);

        *target = glm::mix(start, end, easedT);
        return elapsed >= duration;
    }

    void reset() override {
        elapsed = 0.0f;
        isStarted = false;
        // if (target != nullptr) {
        //     *target = start;
        // }
    }

    void forceInitialStart() override {
        if (target != nullptr) {
            *target = initialStart;
        }
    }
};

class TweenDelay : public TweenPropertyBase {
public:
    float duration;
    float elapsed = 0.0f;

    TweenDelay(float duration) : duration(duration) {}

    bool update(float deltaTime) override {
        elapsed += deltaTime;
        return elapsed >= duration;
    }

    void reset() override { elapsed = 0.0f; }
    void forceInitialStart() override {}
};

class TweenCallback : public TweenPropertyBase {
public:
    std::function<void()> callback;
    bool executed = false;

    TweenCallback(const std::function<void()>& callback) : callback(callback) {}

    bool update(float deltaTime) override {
        if (!executed) {
            if (callback) callback();
            executed = true;
        }
        return true;
    }

    void reset() override { executed = false; }
    void forceInitialStart() override {}
};

class Tween {
public:
    Wesen * owner = nullptr;

    template<typename T>
    Tween* tweenProperty(T* target, const T& endVal, float duration, EaseType ease = EaseType::LINEAR) {
        auto prop = std::make_unique<TweenProperty<T>>(target, endVal, duration, ease);

        if (parallelMode && !steps.empty()) {
            steps.back().push_back(std::move(prop));
        } else {
            std::vector<std::unique_ptr<TweenPropertyBase>> newStep;
            newStep.push_back(std::move(prop));
            steps.push_back(std::move(newStep));
        }

        parallelMode = false;
        return this;
    }

    Tween* setOwner(Wesen* ownerEntity) {
        this->owner = ownerEntity;
        return this;
    }

    Tween* tweenInterval(float duration);
    Tween* tweenCallback(const std::function<void()>& callback);

    Tween* parallel();
    Tween* setLoops(int loopCount = -1);
    bool update(float deltaTime);

private:
    void addProperty(std::unique_ptr<TweenPropertyBase> prop);

    std::vector<std::vector<std::unique_ptr<TweenPropertyBase>>> steps;
    unsigned int currentStepIndex = 0;
    bool parallelMode = false;

    int maxLoops = 1;
    int currentLoop = 0;
};

class TweenManager {
public:
    static TweenManager& getInstance() {
        static TweenManager instance;
        return instance;
    }

    void killTweensOwnedBy(const Wesen* ownerEntity);

    Tween* createTween();
    void update(float deltaTime);
    void cleanup();

private:
    TweenManager() = default;
    std::vector<std::unique_ptr<Tween>> activeTweens {};
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TWEEN_H
