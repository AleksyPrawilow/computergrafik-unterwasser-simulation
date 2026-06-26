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
#include <functional>


class Player : public Wesen {
public:
    RayCast * raycast = nullptr;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void processInput(float deltaTime);
    void setActive(bool active);
    [[nodiscard]] bool getActive() const { return isActive; }
    bool affectedByWater = true;
    bool benutzeInsel = true;
    float bodenHoehe = 0.0f;
    float targetY = 0.0f;
    float leben = 100.0f;
    float schadenBlitz = 0.0f;
    float spawnSchutz = 0.0f;
    glm::vec3 spawnPosition = glm::vec3(0.0f);

    void schadenNehmen(float schaden);
    [[nodiscard]] float getLeben() const { return leben; }
    std::function<void()> onTodessternBenutzt;
    [[nodiscard]] bool hatSpawnSchutz() const { return spawnSchutz > 0.0f; }
private:
    Wesen * rayTargetHelper = nullptr;
    Island * island = nullptr;
    Timer * raycastTimer = nullptr;
    bool isActive = true;

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

    float buffTimer = 0.0f;
    float speedMultiplier = 1.0f;
    float jumpMultiplier = 1.0f;

    Timer* schussTimer = nullptr;
    bool kannSchiessen = true;

    bool istRollend = false;
    float rollZeit = 0.0f;
    float rollDauer = 0.4f;
    float rollGeschwindigkeit = 25.0f;
    float rollWinkel = 0.0f;
    glm::vec3 rollRichtung = glm::vec3(0.0f);
    float rollSprintMult = 1.0f;
    float rollKippRichtung = 1.0f;
    Timer* rollTimer = nullptr;
    bool rollAbklingzeit = false;

    void handleRotations(GLFWwindow* window, float deltaTime);
    void updateCameraTransform(Transform& cameraTransform, float deltaTime) const;
    void updateRaycast();
    void handleItemAction(GLFWwindow* window);
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLAYER_H
