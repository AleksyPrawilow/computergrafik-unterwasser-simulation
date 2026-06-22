//
// Created by Alexey Pravilov on 22/06/2026.
//

#include "bubbleEmitter.h"

BubbleEmitter::BubbleEmitter(unsigned int maxParticles): ParticleEmitter(maxParticles) {
    spawnInterval = 0.02f; // Emit bubbles moderately
}

void BubbleEmitter::onEmit(CPUParticle& p) {
    glm::mat4 globalMatrix = getGlobalModelMatrix();

    // Spawn at the center of the rotor in world-space
    p.position = glm::vec3(globalMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Shoot backwards relative to the rotor's local negative Z-axis (propeller slipstream)
    glm::vec3 localVelocity = glm::vec3(
        (rand() % 100 / 1000.0f) - 0.05f, // Slight horizontal dispersion
        (rand() % 100 / 1000.0f) - 0.05f, // Slight vertical dispersion
        -1.5f - (rand() % 100 / 100.0f)   // Propelled backwards along -Z
    );

    // Transform the local velocity into world-space
    p.velocity = glm::vec3(globalMatrix * glm::vec4(localVelocity, 0.0f));

    p.scale = glm::vec2(0.075f); // Maintain the small bubble scale
    p.maxLife = 1.0f;
    p.life = 1.0f;
}

void BubbleEmitter::onUpdateParticle(CPUParticle& p, float deltaTime) {
    float dragFactor = 4.0f;
    p.velocity.x *= glm::exp(-dragFactor * deltaTime);
    p.velocity.z *= glm::exp(-dragFactor * deltaTime);

    float targetRise = 1.8f; // Float upwards
    p.velocity.y = glm::mix(p.velocity.y, targetRise, 1.0f - glm::exp(-2.0f * deltaTime));

    p.position += p.velocity * deltaTime;

    // Apply dynamic wave wobble
    float speedDamping = glm::exp(-dragFactor * (p.maxLife - p.life));
    p.position.x += sin(p.life * 6.0f) * 0.05f * speedDamping * deltaTime;
    p.position.z += cos(p.life * 6.0f) * 0.05f * speedDamping * deltaTime;
}

void BubbleEmitter::onSetupUniforms(GLuint shaderID) const {
    Kern::setUniform(shaderID, "u_particleType", 0); // 0 = Bubbles
}
