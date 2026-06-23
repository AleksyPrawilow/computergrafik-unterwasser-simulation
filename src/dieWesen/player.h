//
// Created by Alexey Pravilov on 17/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLAYER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLAYER_H
#include "island.h"
#include "timer.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/physics/raycast.h"


class Player : public Wesen {
public:
    RayCast * raycast = nullptr;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void processInput(float deltaTime);
    void setActive(bool active);
private:
    Wesen * rayTargetHelper = nullptr;
    Island * island = nullptr;
    Timer * raycastTimer = nullptr;
    bool isActive = true;

    float targetY = 0.0f;
    float verticalVelocity = 0.0f;
    float bobTime = 0.0f;
    bool grounded = true;
    glm::vec3 direction;

    float yaw = 0.0f;
    float pitch = 0.0f;

    double lastX = 0.0;
    double lastY = 0.0;
    bool firstMouse = true;

    bool kannPlatzieren = true;

    void handleRotations(GLFWwindow* window, float deltaTime);
    void updateCameraTransform(Transform& cameraTransform, float deltaTime) const;
    void updateRaycast();
    void handleItemAction(GLFWwindow* window);
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLAYER_H
