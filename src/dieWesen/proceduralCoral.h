//
// Created by Alexey Pravilov on 25/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PROCEDURALCORAL_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PROCEDURALCORAL_H
#include "werkzeuge/renderWerkzeuge.h"
#include "werkzeuge/wesen.h"
#include <vector>
#include <glm.hpp>

namespace Kern {
    struct CoralVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    class ProceduralCoral {
    public:
        static RenderContext generate(int seed, AABB& outAABB);

    private:
        static void generateBranch(
            const glm::vec3& start,
            const glm::vec3& direction,
            float length,
            float radius,
            int generation,
            int maxGenerations,
            std::vector<CoralVertex>& vertices,
            std::vector<unsigned int>& indices,
            AABB& outAABB
        );
    };
}


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PROCEDURALCORAL_H
