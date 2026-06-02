//
// Created by s498780 on 01.06.2026.
//

#include "glm.hpp"

#include "transform.h"

glm::quat Transform::quatLookAt(const glm::vec3& direction, const glm::vec3& worldUp) {
    // glm::vec3 forward = glm::normalize(direction);
    // glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
    // glm::vec3 up = glm::cross(forward, right);
    // glm::mat3 rot(
    //     right,
    //     up,
    //     -forward
    // );
    //
    // return glm::quat_cast(rot);
    glm::mat4 view =
        glm::lookAt(
            glm::vec3(0.0f),
            direction,
            worldUp
        );

    return glm::normalize(glm::quat_cast(glm::inverse(view)));
}

glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 T = glm::translate(glm::mat4(1.f), position);
    glm::mat4 R = glm::toMat4(rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.f), scale);
    return T * R * S;
}

glm::vec3 Transform::forward() const { return rotation * glm::vec3(0, 0, -1); }

glm::vec3 Transform::right() const { return rotation * glm::vec3(1, 0, 0); }

glm::vec3 Transform::up() const { return rotation * glm::vec3(0, 1, 0); }

void Transform::lookAt(const glm::vec3& target, const glm::vec3& worldUp) {
    glm::vec3 f = glm::normalize(target - position);
    rotation = quatLookAt(f, worldUp);
}

void Transform::yaw(float angle) {
    rotation = glm::angleAxis(angle, up()) * rotation;
    rotation = glm::normalize(rotation);
}

void Transform::pitch(float angle) {
    rotation = glm::angleAxis(angle, right()) * rotation;
}

void Transform::roll(float angle) {
    rotation = glm::angleAxis(angle, forward()) * rotation;
}
