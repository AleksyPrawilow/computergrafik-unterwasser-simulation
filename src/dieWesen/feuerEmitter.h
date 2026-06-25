#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FEUEREMITTER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FEUEREMITTER_H

#include "particleEmitter.h"

class FeuerEmitter : public ParticleEmitter {
public:
    FeuerEmitter(unsigned int maxParticles, float streuung = 1.0f);
    void onEmit(CPUParticle& p) override;
    void onUpdateParticle(CPUParticle& p, float deltaTime) override;
    void onSetupUniforms(GLuint shaderID) const override;

    Wesen* zielObjekt = nullptr;
    glm::vec3 offset = glm::vec3(0.0f);

protected:
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    float streuung;
};

#endif
