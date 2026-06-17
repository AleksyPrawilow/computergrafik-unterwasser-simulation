//
// Created by Alexey Pravilov on 14/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WORLDENVIRONMENT_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WORLDENVIRONMENT_H


#include "../wesen.h"
#include <glm.hpp>

struct EnvParameters {
    glm::vec3 sunDirection = glm::normalize(glm::vec3(0.1f, 1.0f, 0.15f));
    glm::vec3 sunColor     = glm::vec3(1.0f, 0.95f, 0.9f);
    float sunEnergy        = 1.0f;

    glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientEnergy    = 0.12f;

    bool fogEnabled    = true;
    glm::vec3 fogColor = glm::vec3(0.002f, 0.015f, 0.06f);
    float fogDensity   = 0.035f;

    bool heightFogEnabled    = true;
    glm::vec3 heightFogColor = glm::vec3(0.02f, 0.22f, 0.28f);
    float heightFogMin       = -45.0f;
    float heightFogMax       = 0.0f;

    bool causticsEnabled     = true;
    glm::vec3 causticsColor  = glm::vec3(0.5f, 0.85f, 1.0f);
    float causticsScale      = 0.12f;
    float causticsIntensity  = 1.5f;

    bool depthDimmingEnabled = true;
    float depthDimmingCoefficient = 0.02f;
};

class WorldEnvironment : public Wesen {
public:
    static WorldEnvironment* activeEnv;
    EnvParameters params;

    void init() override;
    ~WorldEnvironment() override;

    [[nodiscard]] bool hasCustomRender() const override { return true; }
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override {}
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WORLDENVIRONMENT_H
