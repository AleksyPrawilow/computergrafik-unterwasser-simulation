//
// Created by Alexey Pravilov on 07/06/2026.
//

#include "particleEmitter.h"
#include "gtc/type_ptr.hpp"
#include <GLFW/glfw3.h>
#include "werkzeuge/shaderManager.h"

ParticleEmitter::ParticleEmitter(unsigned int maxParticles) : maxParticlesCount(maxParticles) {
    shader = ShaderManager::getInstance().loadShader("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");

    float quadVertices[] = {
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &quadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glGenBuffers(1, &instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(GPUParticle), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)0);
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(offsetof(GPUParticle, scale)));
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
}

ParticleEmitter::~ParticleEmitter() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &quadVbo);
    glDeleteBuffers(1, &instanceVbo);
}

void ParticleEmitter::emit() {
    if (particles.size() >= maxParticlesCount) return;

    // --- MATHEMATICAL GENIUS PORTION ---
    // Convert local emission offset (e.g. at the back of the sub) to WORLD space
    // using the inherited global matrix!
    glm::mat4 globalMatrix = getGlobalModelMatrix();
    glm::vec3 globalOrigin = glm::vec3(globalMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Convert local velocity direction to WORLD space (W = 0.0f ignores parent position, only rotates/scales)
    glm::vec3 localVelocity = glm::vec3(
        (rand() % 100 / 1000.0f) - 0.05f,
        (rand() % 100 / 1000.0f) - 0.05f,
        -1.5f - (rand() % 100 / 100.0f) // Shoot backward relative to parent front
    );
    glm::vec3 globalVelocity = glm::vec3(globalMatrix * glm::vec4(localVelocity, 0.0f));

    float scale = 0.035f;
    float life = 1.0f;

    particles.push_back({ globalOrigin, globalVelocity, life, life, scale });
}

void ParticleEmitter::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    // 1. Spawning logic
    if (active) {
        spawnTimer += deltaTime;
        while (spawnTimer >= 0.02f) { // Emit a bubble every 20ms
            spawnTimer -= 0.02f;
            emit();
        }
    }

    // 2. Update active world-space particles
    gpuData.clear();
    for (auto it = particles.begin(); it != particles.end();) {
        it->life -= deltaTime;
        if (it->life <= 0.0f) {
            it = particles.erase(it);
        } else {
            // 1. --- APPLY FLUID DRAG & BUOYANCY ---
            // Damp horizontal and forward velocity rapidly to 0.0f (water drag)
            float dragFactor = 4.0f; // Higher values make them slow down faster
            it->velocity.x *= glm::exp(-dragFactor * deltaTime);
            it->velocity.z *= glm::exp(-dragFactor * deltaTime);

            // Buoyancy: Smoothly transition vertical speed to a gentle rising speed (e.g. 1.8 m/s)
            float riseTarget = 1.8f;
            it->velocity.y = glm::mix(it->velocity.y, riseTarget, 1.0f - glm::exp(-2.0f * deltaTime));
            // --------------------------------------

            // Move particle
            it->position += it->velocity * deltaTime;

            // Wobble physics (decreases as the particle slows down)
            float speedDamping = glm::exp(-dragFactor * (it->maxLife - it->life));
            it->position.x += sin(it->life * 6.0f) * 0.05f * speedDamping * deltaTime;
            it->position.z += cos(it->life * 6.0f) * 0.05f * speedDamping * deltaTime;

            gpuData.push_back({ it->position, it->scale });
            ++it;
        }
    }
}

void ParticleEmitter::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (gpuData.empty()) return;

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gpuData.size() * sizeof(GPUParticle), gpuData.data());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, gpuData.size());

    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glUseProgram(0);
}