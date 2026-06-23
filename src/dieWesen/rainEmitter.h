//
// Created by Alexey Pravilov on 22/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAINEMITTER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAINEMITTER_H

#include "particleEmitter.h"

class RainEmitter : public ParticleEmitter {
public:
    RainEmitter(unsigned int maxParticles);
    void onEmit(CPUParticle& p) override;
    void onUpdateParticle(CPUParticle& p, float deltaTime) override;
    void onSetupUniforms(GLuint shaderID) const override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAINEMITTER_H
