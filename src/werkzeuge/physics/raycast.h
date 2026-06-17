//
// Created by Alexey Pravilov on 09/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAYCAST_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAYCAST_H

#include <glm.hpp>

#include "werkzeuge/wesen.h"

class RayCast : public Wesen {
public:
    glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, -15.0f);

    bool isColliding() const { ensureUpdated(); return colliding; }
    Wesen* getCollider() const { ensureUpdated(); return collider; }
    glm::vec3 getCollisionPoint() const { ensureUpdated(); return collisionPoint; }
    glm::vec3 getCollisionNormal() const { ensureUpdated(); return collisionNormal; }

    void forceRaycastUpdate() const { isDirty = true; ensureUpdated(); }

    bool hasCustomRender() const override { return true; }
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override {}
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override {
        isDirty = true;
    }
private:
    void ensureUpdated() const;
    mutable bool isDirty = true;
    mutable bool colliding = false;
    mutable Wesen* collider = nullptr;
    mutable glm::vec3 collisionPoint = glm::vec3(0.0f);
    mutable glm::vec3 collisionNormal = glm::vec3(0.0f);
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAYCAST_H
