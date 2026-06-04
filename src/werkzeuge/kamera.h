//
// Created by Alexey Pravilov on 02/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_KAMERA_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_KAMERA_H


#include "glm.hpp"
#include "transform.h"

class Kamera {
public:
    Kamera();
    ~Kamera();

    Transform transform;

    float nearPlane = 0.05f;
    float farPlane = 1000.0f;

    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::mat4 getProjectionMatrix() const;

    void setAspectRatio(float ratio);
    [[nodiscard]] float getAspectRatio() const;

private:
    float aspectRatio = 1.7777777778f;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_KAMERA_H
