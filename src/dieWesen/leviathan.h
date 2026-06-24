//
// Created by Alexey Pravilov on 24/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LEVIATHAN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LEVIATHAN_H
#include "lightsaber.h"
#include "unterwasserszeneAudioHelper.h"
#include "werkzeuge/wesen.h"


class Leviathan : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void prepareUniforms() const override;

    // --- 1. CONFIGURABLE SWIMMING PARAMETERS (Body) ---
    float u_swimSpeed = 4.5;
    float u_swimFrequency = 0.075;
    float u_swimAmplitude = 7.0;

    float u_neckPitch = 0.0;    // Up/Down rotation in radians
    float u_neckYaw = 0.0;      // Left/Right rotation in radians
    float u_neckPivotZ = 0.0;   // Local Z-coordinate where the neck begins bending
    float u_neckLength = 2.5;   // Total length of the neck/head section
    float u_emissionPulse = 1.0f;
    Wesen * lightsabers[4] = { nullptr };
    glm::vec3 startClawPositions[4];

    float clawXOffset = 8.5f;
    float clawYOffset = 3.3f;
    float clawZOffset = 20.7f;
    float lightsabersYOffset = -5.0f;
private:
    UnterwasserszeneAudioHelper * music = nullptr;

    float moveSpeed = 9.0f;
    float idleSpeed = 2.0f;
    float u_spiralRadius = 2.2f;
    float u_spiralFrequency = 2.5f;
    float maxRollOffset = 0.5f;
    glm::vec3 basePosition = glm::vec3(0.0f);
    glm::vec3 patrolCenter = glm::vec3(0.0f);
    float elapsedTime = 0.0f;
    bool isChasing = false;
    bool wasChasing = false;

    float attackCooldown = 0.0f;
    float attackDamage = 25.0f;
    float attackInterval = 2.0f;

    Wesen* findClosestTarget() const;
    void chase(float deltaTime, const glm::vec3& toTarget, float distance);
    void patrol(float deltaTime);
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LEVIATHAN_H
