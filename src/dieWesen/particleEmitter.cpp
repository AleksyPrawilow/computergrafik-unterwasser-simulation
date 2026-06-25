//
// Created by Alexey Pravilov on 07/06/2026.
//
#include "particleEmitter.h"
#include "gtc/type_ptr.hpp"
#include <GLFW/glfw3.h>
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/renderWerkzeuge.h"

ParticleEmitter::ParticleEmitter(unsigned int maxParticles) : maxParticlesCount(maxParticles) {
    shader = ShaderManager::getInstance().loadShader("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
    material.isTransparent = true;
    material.isUI = false;

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

    // Layout location 2: vec3 position
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)0);
    glVertexAttribDivisor(2, 1);

    // Layout location 3: vec2 scale (width, height)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(offsetof(GPUParticle, scale)));
    glVertexAttribDivisor(3, 1);

    // Layout location 4: float life
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(offsetof(GPUParticle, life)));
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
}

ParticleEmitter::~ParticleEmitter() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &quadVbo);
    glDeleteBuffers(1, &instanceVbo);
}

void ParticleEmitter::emit() {
    if (particles.size() >= maxParticlesCount) return;

    CPUParticle p;
    // Delegate initial properties assignment to the active subclass
    onEmit(p);

    particles.push_back(p);
}

void ParticleEmitter::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (active) {
        spawnTimer += deltaTime;
        while (spawnTimer >= spawnInterval) {
            spawnTimer -= spawnInterval;
            emit();
        }
    }

    gpuData.clear();
    for (auto it = particles.begin(); it != particles.end();) {
        it->life -= deltaTime;
        if (it->life <= 0.0f) {
            it = particles.erase(it);
        } else {
            // Delegate physics updates to the active subclass
            onUpdateParticle(*it, deltaTime);

            gpuData.push_back({ it->position, it->scale, it->life / it->maxLife });
            ++it;
        }
    }
}

void ParticleEmitter::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (gpuData.empty()) return;

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // Delegate custom shader configurations to the subclass
    onSetupUniforms(shader);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gpuData.size() * sizeof(GPUParticle), gpuData.data());

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, gpuData.size());

    glUseProgram(0);
}