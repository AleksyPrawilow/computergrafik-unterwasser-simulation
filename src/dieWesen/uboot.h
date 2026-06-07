//
// Created by Alexey Pravilov on 02/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SUBMARINE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SUBMARINE_H
#include "particleEmitter.h"
#include "ubootRotor.h"
#include "../werkzeuge/wesen.h"
#include "../werkzeuge/transform.h"
#include "../werkzeuge/lightManager.h"

struct CPUWave {
    glm::vec2 direction;
    float amplitude;
    float wavelength;
    float speed;
};

class Uboot: public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void prepareUniforms() const override;
private:
    static float getWaterHeight(float x, float z, float t);
    SpotLight * spotlight{};
    ParticleEmitter* emitters[4] = {nullptr};
    UbootRotor * rotors[4] {nullptr};
    float actualMoveSpeed = 0.0f;
    float actualRotorSpeed = 0.0f;

    // Mouse state tracking
    double lastX = 0.0;
    double lastY = 0.0;
    bool firstMouse = true;

    // Movement velocities (inertia)
    float pitchVelocity = 0.0f;
    float rollVelocity = 0.0f;
    float yawVelocity = 0.0f;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SUBMARINE_H
