#include "feuerEmitter.h"
#include "werkzeuge/random.h"
#include "werkzeuge/renderWerkzeuge.h"

FeuerEmitter::FeuerEmitter(unsigned int maxParticles, float streuung)
    : ParticleEmitter(maxParticles), streuung(streuung) {
    spawnInterval = 0.01f;
    material.bloomStrength = 0.4f;
}

void FeuerEmitter::onEmit(CPUParticle& p) {
    glm::mat4 globalMatrix = getGlobalModelMatrix();
    glm::vec3 center = glm::vec3(globalMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    p.position = center + glm::vec3(
        Random::range(-streuung, streuung),
        Random::range(-streuung * 0.5f, streuung * 0.5f),
        Random::range(-streuung, streuung)
    );

    p.velocity = glm::vec3(
        Random::range(-2.0f, 2.0f),
        Random::range(3.0f, 8.0f),
        Random::range(-2.0f, 2.0f)
    );

    float size = Random::range(streuung * 0.3f, streuung * 0.8f);
    p.scale = glm::vec2(size);
    p.maxLife = Random::range(0.8f, 2.0f);
    p.life = p.maxLife;
}

void FeuerEmitter::onUpdateParticle(CPUParticle& p, float deltaTime) {
    p.position += p.velocity * deltaTime;
    p.velocity.y += 1.5f * deltaTime;

    float t = 1.0f - (p.life / p.maxLife);
    p.scale *= (1.0f + 0.5f * deltaTime);
}

void FeuerEmitter::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (zielObjekt != nullptr && !zielObjekt->isQueuedDestroyed) {
        transform.position = zielObjekt->getGlobalTransform().position + offset;
    }
    ParticleEmitter::onUpdate(window, deltaTime, cameraTransform);
}

void FeuerEmitter::onSetupUniforms(GLuint shaderID) const {
    Kern::setUniform(shaderID, "u_particleType", 2);
    Kern::setUniform(shaderID, "u_bloomStrength", material.bloomStrength);
}
