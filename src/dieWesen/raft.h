//
// Created by Alexey Pravilov on 17/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAFT_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAFT_H
#include "werkzeuge/wesen.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/audio/audioPlayer.h"

class Player;
class Fadenkreuz;

class Raft : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    bool shouldFloat = true;
    Player * player = nullptr;
private:
    bool isActive = false;
    Fadenkreuz * crosshair = nullptr;
    Wesen * crosshairPivot = nullptr;
    UILabel * interactLabel = nullptr;

    float moveSpeed = 6.0f;
    float moveSpeedBackward = 3.0f;
    float turnSpeed = 1.5f;

    double lastX = 0.0;
    double lastY = 0.0;
    bool firstMouse = true;
    float currentPitch = 0.0f;

    glm::vec2 halfSize = {2.0f, 4.0f};
    float tiltStrength = 0.15f;
    float heightSmooth = 8.0f;
    float rotSmooth = 6.0f;
    float currentYaw = 0.0f;

    bool isFishing = false;
    AudioPlayer * fishSound = nullptr;
    Wesen * rodVisual = nullptr;
    glm::vec3 rodIdlePos = glm::vec3(0.6f, 5.8f, -1.5f);
    glm::vec3 rodIdleEuler = glm::vec3(30.0f, 0.0f, 0.0f);
    glm::vec3 rodEuler = glm::vec3(30.0f, 0.0f, 0.0f);

    void updateRodVisual();
    void cancelFishing();

    void processInput(GLFWwindow* window, float deltaTime);
    void fish();
    void updateWavePhysics(float deltaTime);
    void updateCameraTransform(Transform& cameraTransform, float deltaTime) const;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAFT_H
