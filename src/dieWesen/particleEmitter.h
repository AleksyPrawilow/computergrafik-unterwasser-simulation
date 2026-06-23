//
// Created by Alexey Pravilov on 07/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PARTICLEEMITTER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PARTICLEEMITTER_H

#include <vector>
#include "glew.h"
#include <glm.hpp>
#include "werkzeuge/wesen.h"

struct GPUParticle {
    glm::vec3 position;
    glm::vec2 scale; // x = width, y = height (supports stretched particles)
};

struct CPUParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life;
    float maxLife;
    glm::vec2 scale;
};

class ParticleEmitter : public Wesen {
public:
    ParticleEmitter(unsigned int maxParticles);
    ~ParticleEmitter() override;

    bool active = false;
    float spawnInterval = 0.02f; // Configurable spawn frequency

    // Virtual hooks for child classes to define custom behaviors
    virtual void onEmit(CPUParticle& p) = 0;
    virtual void onUpdateParticle(CPUParticle& p, float deltaTime) = 0;
    virtual void onSetupUniforms(GLuint shaderID) const {}

    bool hasCustomRender() const override { return true; }
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;

protected:
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    void emit();

    std::vector<CPUParticle> particles;
    mutable std::vector<GPUParticle> gpuData;

    GLuint vao = 0;
    GLuint quadVbo = 0;
    mutable GLuint instanceVbo = 0;
    GLuint shader = 0;
    unsigned int maxParticlesCount;
    float spawnTimer = 0.0f;
};

#endif