#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FISHBOID_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FISHBOID_H

#include "werkzeuge/wesen.h"
#include "glm.hpp"

class FishBoid : public Wesen {
public:
    glm::vec3 velocity;
    glm::vec3 acceleration;

    float maxSpeed = 8.0f;
    float maxForce = 4.0f;
    float neighborRadius = 6.0f;
    float separationRadius = 1.8f;

    float separationWeight = 1.5f;
    float alignmentWeight = 1.0f;
    float cohesionWeight = 1.0f;

    float playerAttractionWeight = 1.2f;
    float playerOrbitWeight = 1.8f;
    float targetOrbitRadius = 15.0f;

    float rotationOffsetY = 180.0f;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void prepareUniforms() const override;

private:
    glm::vec3 calculateSeparation(const std::vector<Wesen*>& neighbors);
    glm::vec3 calculateAlignment(const std::vector<Wesen*>& neighbors);
    glm::vec3 calculateCohesion(const std::vector<Wesen*>& neighbors);

    glm::vec3 calculatePlayerInteraction(Transform& cameraTransform);

    glm::vec3 limit(const glm::vec3& v, float maxVal);

    float randTimeOffset = 0.0f;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FISHBOID_H