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
    public:
        glm::vec3 position{0.f};
        glm::quat rotation{1.f, 0.f, 0.f, 0.f};
        glm::vec3 scale{1.f};

        static glm::quat quatLookAt(const glm::vec3& direction, const glm::vec3& worldUp);

        glm::mat4 getModelMatrix() const;

        glm::vec3 forward() const;
        glm::vec3 right()   const;
        glm::vec3 up()      const;

        void lookAt(const glm::vec3& target, const glm::vec3& worldUp = {0,1,0});
        void yaw(float angle);
        void pitch(float angle);
        void roll(float angle);
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TRANSFORM_H