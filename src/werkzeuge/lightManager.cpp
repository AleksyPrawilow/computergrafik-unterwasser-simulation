//
// Created by Alexey Pravilov on 07/06/2026.
//

#include "lightManager.h"

PointLight* LightManager::createPointLight(const glm::vec3& color, float intensity) {
    auto * light = new PointLight{ glm::vec3(0), color, intensity };
    pointLights.push_back(light);
    return light;
}

SpotLight* LightManager::createSpotLight(const glm::vec3& color, const float intensity, const float cutOffAngle, const float outerCutOffAngle) {
    auto * light = new SpotLight{
        glm::vec3(0), glm::vec3(0, 0, -1), color, intensity,
        glm::cos(glm::radians(cutOffAngle)), glm::cos(glm::radians(outerCutOffAngle))
    };
    spotLights.push_back(light);
    return light;
}

void LightManager::cleanup() {
    for (const auto* l : pointLights) delete l;
    for (const auto* l : spotLights) delete l;
    pointLights.clear();
    spotLights.clear();
}
