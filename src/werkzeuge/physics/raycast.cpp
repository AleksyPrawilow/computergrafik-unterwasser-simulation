//
// Created by Alexey Pravilov on 09/06/2026.
//

#include "raycast.h"
#include <limits>
#include <vector>
#include <algorithm>
#include "werkzeuge/wesen.h"

extern Wesen * scene;

bool IntersectRayAABB(
    const glm::vec3& ro,
    const glm::vec3& rd,
    const glm::vec3& boxMin,
    const glm::vec3& boxMax,
    float& out_t
) {
    float tmin = -1e30f;
    float tmax = 1e30f;

    if (glm::abs(rd.x) > 1e-6f) {
        float t1 = (boxMin.x - ro.x) / rd.x;
        float t2 = (boxMax.x - ro.x) / rd.x;
        tmin = glm::max(tmin, glm::min(t1, t2));
        tmax = glm::min(tmax, glm::max(t1, t2));
    } else if (ro.x < boxMin.x || ro.x > boxMax.x) {
        return false;
    }

    if (glm::abs(rd.y) > 1e-6f) {
        float t1 = (boxMin.y - ro.y) / rd.y;
        float t2 = (boxMax.y - ro.y) / rd.y;
        tmin = glm::max(tmin, glm::min(t1, t2));
        tmax = glm::min(tmax, glm::max(t1, t2));
    } else if (ro.y < boxMin.y || ro.y > boxMax.y) {
        return false;
    }

    if (glm::abs(rd.z) > 1e-6f) {
        float t1 = (boxMin.z - ro.z) / rd.z;
        float t2 = (boxMax.z - ro.z) / rd.z;
        tmin = glm::max(tmin, glm::min(t1, t2));
        tmax = glm::min(tmax, glm::max(t1, t2));
    } else if (ro.z < boxMin.z || ro.z > boxMax.z) {
        return false;
    }

    if (tmax >= tmin && tmax >= 0.0f) {
        out_t = tmin < 0.0f ? tmax : tmin;
        return true;
    }
    return false;
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

    if (entity != raycastParent && entity->parent != raycastParent && entity->hasMesh && entity->isCollidable) {
        glm::mat4 invModel = glm::inverse(entity->getGlobalModelMatrix());
        glm::vec3 localRo = glm::vec3(invModel * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localRd = glm::normalize(glm::vec3(invModel * glm::vec4(rayDirection, 0.0f)));

        float localT = 0.0f;
        if (IntersectRayAABB(localRo, localRd, entity->localAABB.min, entity->localAABB.max, localT)) {
            glm::vec3 localHitPoint = localRo + localRd * localT;
            glm::vec3 worldHitPoint = glm::vec3(entity->getGlobalModelMatrix() * glm::vec4(localHitPoint, 1.0f));

            float worldT = glm::distance(rayOrigin, worldHitPoint);

            if (worldT <= maxRange && worldT < closestHitT) {
                closestHitT = worldT;
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