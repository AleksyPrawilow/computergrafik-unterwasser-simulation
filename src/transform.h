//
// Created by s498780 on 01.06.2026.
//
#pragma once
#include <GLFW/glfw3.h>
#include "glm.hpp"

#include "gtc/matrix_transform.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/quaternion.hpp"

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TRANSFORM_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TRANSFORM_H


class transform {
    glm::vec3 position{0.f};
    glm::quat rotation{1.f, 0.f, 0.f, 0.f};
    glm::vec3 scale{1.f};

    glm::mat4 getModelMatrix() const {
        glm::mat4 T = glm::translate(glm::mat4(1.f), position);
        glm::mat4 R = glm::toMat4(rotation);
        glm::mat4 S = glm::scale(glm::mat4(1.f), scale);
        return T * R * S;
    }

    glm::vec3 forward() const { return rotation * glm::vec3(0, 0, -1); }
    glm::vec3 right()   const { return rotation * glm::vec3(1, 0, 0); }
    glm::vec3 up()      const { return rotation * glm::vec3(0, 1, 0); }

    void lookAt(const glm::vec3& target, const glm::vec3& worldUp = {0,1,0}) {
        glm::vec3 f = glm::normalize(target - position);
        rotation = glm::quatLookAt(f, worldUp);
    }
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TRANSFORM_H