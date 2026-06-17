//
// Created by Alexey Pravilov on 09/06/2026.
//

#include "raycast.h"
#include <limits>
#include <vector>

#include "werkzeuge/wesen.h"

extern Wesen * scene;

bool IntersectRaySphere(const glm::vec3& ro, const glm::vec3& rd, const glm::vec3& sCenter, const float radius, float& out_t) {
    const glm::vec3 l = sCenter - ro;
    const float tca = glm::dot(l, rd);
    if (tca < 0) return false;

    const float d2 = glm::dot(l, l) - tca * tca;
    const float r2 = radius * radius;
    if (d2 > r2) return false;

    const float thc = glm::sqrt(r2 - d2);
    out_t = tca - thc;
    return true;
}

void CheckRayCollisionRecursive(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection,
    const float maxRange,
    Wesen* entity,
    Wesen* raycastParent,
    float& closestHitT,
    Wesen*& closestHitEntity
) {
    if (entity == nullptr) return;

    if (entity != raycastParent && entity->parent != raycastParent && entity->boundingRadius > 0.0f) {
        const glm::vec3 worldPos = entity->getGlobalTransform().position;

        if (float t = 0.0f; IntersectRaySphere(rayOrigin, rayDirection, worldPos, entity->boundingRadius, t)) {
            if (t <= maxRange && t < closestHitT) {
                closestHitT = t;
                closestHitEntity = entity;
            }
        }
    }

    for (Wesen* child : entity->children) {
        CheckRayCollisionRecursive(rayOrigin, rayDirection, maxRange, child, raycastParent, closestHitT, closestHitEntity);
    }
}

void RayCast::ensureUpdated() const {
    if (!isDirty) return;

    colliding = false;
    collider = nullptr;
    collisionPoint = glm::vec3(0.0f);
    collisionNormal = glm::vec3(0.0f);

    Transform globalTransform = getGlobalTransform();
    glm::vec3 rayOrigin = globalTransform.position;
    glm::vec3 globalTargetVector = globalTransform.rotation * targetPosition;
    float maxRange = glm::length(globalTargetVector);
    glm::vec3 rayDirection = glm::normalize(globalTargetVector);

    float closestHitT = std::numeric_limits<float>::max();
    Wesen* closestHitEntity = nullptr;

    for (Wesen* rootEntity : scene->children) {
        CheckRayCollisionRecursive(
            rayOrigin,
            rayDirection,
            maxRange,
            rootEntity,
            parent,
            closestHitT,
            closestHitEntity
        );
    }

    if (closestHitEntity != nullptr) {
        colliding = true;
        collider = closestHitEntity;
        collisionPoint = rayOrigin + rayDirection * closestHitT;

        glm::vec3 hitWorldPos = closestHitEntity->getGlobalTransform().position;
        collisionNormal = glm::normalize(collisionPoint - hitWorldPos);
    }

    isDirty = false;
}