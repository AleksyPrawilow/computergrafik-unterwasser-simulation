//
// Created by Alexey Pravilov on 02/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SUBMARINE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SUBMARINE_H
#include "particleEmitter.h"
#include "../werkzeuge/physics/raycast.h"
#include "timer.h"
#include "ubootHeadlight.h"
#include "ubootRotor.h"
#include "../werkzeuge/wesen.h"
#include "../werkzeuge/transform.h"
#include "../werkzeuge/visual/lightManager.h"
#include "ui/fadenkreuz.h"

class HudPanel;

struct CPUWave {
    glm::vec2 direction;
    float amplitude;
    float wavelength;
    float speed;
};

enum class ViewMode {
    FIRST_PERSON,
    THIRD_PERSON,
    THIRD_PERSON_BACK
};

class Uboot: public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    [[nodiscard]] float getSpeed() const {
        return actualMoveSpeed;
    }
    [[nodiscard]] float getLeben() const { return leben; }
    void schadenNehmen(float schaden);
    void setIsActive(bool active);
    static float getWaterHeight(float x, float z, float t);

    bool shouldFloat = true;
private:
    float leben = 100.0f;
    float spawnSchutz = 0.0f;

    Fadenkreuz * crosshair = nullptr;
    HudPanel * hudPanel = nullptr;
    ViewMode viewMode = ViewMode::THIRD_PERSON;
    UbootHeadlight * headlights[2] = {nullptr};
    Timer * spotlightTimer {};
    ParticleEmitter* emitters[4] = {nullptr};
    UbootRotor * rotors[4] {nullptr};
    float actualMoveSpeed = 0.0f;
    float actualRotorSpeed = 0.0f;
    float angleSpeed = 2.0f;
    float moveSpeed = 10.0f;
    float moveSpeedBackward = 5.0f;
    float targetMoveSpeed = 0.0f;
    float targetRotorSpeed = 0.0f;
    float rotorSpeed = 6.0f;
    float rotorSpeedBackward = -2.5f;
    float targetRollVelocity = 0.0f;
    float targetWaveInfluence = 0.0f;
    float actualWaveInfluence = 0.0f;
    bool isSubmerging = false;
    bool isActive = false;

    // Mouse state tracking
    double lastX = 0.0;
    double lastY = 0.0;
    bool firstMouse = true;

    // Movement velocities (inertia)
    float pitchVelocity = 0.0f;
    float rollVelocity = 0.0f;
    float yawVelocity = 0.0f;

    void processInput(GLFWwindow* window, float deltaTime);
    void updateCameraTransform(Transform& cameraTransform, float deltaTime) const;
    void handleRolls(GLFWwindow* window, float deltaTime);
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SUBMARINE_H
