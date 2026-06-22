//
// Created by Alexey Pravilov on 22/06/2026.
//

#include "rainEmitter.h"

RainEmitter::RainEmitter(unsigned int maxParticles): ParticleEmitter(maxParticles) {
    spawnInterval = 0.0015f; // Emit rain extremely fast
}

void RainEmitter::onEmit(CPUParticle& p) {
    // Emit randomly in a flat 16x16m grid centered above the camera
    float rx = ((rand() % 1600) / 100.0f) - 8.0f;
    float rz = ((rand() % 1600) / 100.0f) - 8.0f;
    p.position = transform.position + glm::vec3(rx, 0.0f, rz);

    // High terminal falling velocity
    p.velocity = glm::vec3(
        ((rand() % 100) / 1000.0f) - 0.05f, // Slight wind drift
        -28.0f - ((rand() % 100) / 10.0f),  // Falls at 28m/s to 38m/s
        ((rand() % 100) / 1000.0f) - 0.05f
    );

    // Stretched scale: X = narrow width (0.015m), Y = elongated length (0.45m)
    p.scale = glm::vec2(0.015f, 0.45f);
    p.maxLife = 0.8f;
    p.life = 0.8f;
}

void RainEmitter::onUpdateParticle(CPUParticle& p, float deltaTime) {
    p.position += p.velocity * deltaTime;

    // Terminate the particle early if it hits the water surface
    if (p.position.y < 0.0f) {
        p.life = 0.0f;
    }
}

void RainEmitter::onSetupUniforms(GLuint shaderID) const {
    Kern::setUniform(shaderID, "u_particleType", 1); // 1 = Rain
}
