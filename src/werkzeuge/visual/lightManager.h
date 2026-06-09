//
// Created by Alexey Pravilov on 07/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LIGHTMANAGER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LIGHTMANAGER_H
#include <vector>
#include "glm.hpp"

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float cutOff;
    float outerCutOff;
};

class LightManager {
public:
    static LightManager& getInstance() {
        static LightManager instance;
        return instance;
    }

    PointLight* createPointLight(const glm::vec3& color, float intensity);

    SpotLight* createSpotLight(const glm::vec3& color, float intensity, float cutOffAngle, float outerCutOffAngle);

    [[nodiscard]] const std::vector<PointLight*>& getPointLights() const { return pointLights; }
    [[nodiscard]] const std::vector<SpotLight*>& getSpotLights() const { return spotLights; }

    void cleanup();


private:
    LightManager() = default;
    ~LightManager() { cleanup(); }

    std::vector<PointLight*> pointLights;
    std::vector<SpotLight*> spotLights;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LIGHTMANAGER_H
