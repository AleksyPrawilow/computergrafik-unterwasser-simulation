//
// Created by Alexey Pravilov on 21/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FRUSTUM_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FRUSTUM_H

#include <glm.hpp>
#include <algorithm>

struct FrustumPlane {
    glm::vec3 normal;
    float distance;

    void normalize() {
        const float length = glm::length(normal);
        normal /= length;
        distance /= length;
    }

    [[nodiscard]] float distanceToPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

class Frustum {
public:
    FrustumPlane planes[6];
    void update(const glm::mat4& vp);
    [[nodiscard]] bool isAABBInside(const glm::vec3& min, const glm::vec3& max) const;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FRUSTUM_H
