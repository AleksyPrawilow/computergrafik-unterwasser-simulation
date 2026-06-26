//
// Created by Alexey Pravilov on 02/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_KAMERA_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_KAMERA_H


#include "glm.hpp"
#include "random.h"
#include "transform.h"

struct CameraShake {
    float intensity = 0.0f;
    float duration = 0.0f;
    float timeLeft = 0.0f;
    glm::vec3 offset{0.0f};

};

class Kamera {
public:
    Kamera();
    ~Kamera();

    Transform transform;

    float nearPlane = 0.05f;
    float farPlane = 5000.0f;

    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::mat4 getProjectionMatrix() const;

    void setAspectRatio(float ratio);
    void addShake(float intensity, float duration);
    void updateShake(float dt);
    [[nodiscard]] float getAspectRatio() const;

private:
    CameraShake shake;
    float aspectRatio = 1.7777777778f;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_KAMERA_H
