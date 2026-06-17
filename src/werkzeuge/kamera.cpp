//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "kamera.h"

Kamera::Kamera() = default;
Kamera::~Kamera() = default;

glm::mat4 Kamera::getViewMatrix() const {
    glm::vec3 pos = transform.position + shake.offset;

    const glm::mat4 R = glm::toMat4(glm::conjugate(transform.rotation));
    const glm::mat4 T = glm::translate(glm::mat4(1.0f), -pos);
    return R * T;
}

glm::mat4 Kamera::getProjectionMatrix() const {
    const float n = nearPlane;
    const float f = farPlane;

    const auto projectionMatrix = glm::mat4({
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, aspectRatio, 0.0f, 0.0f,
        0.0f, 0.0f, (f + n) / (n - f), 2.0f * f * n / (n - f),
        0.0f, 0.0f, -1.0f, 0.0f
    });

    return glm::transpose(projectionMatrix);
}

void Kamera::setAspectRatio(const float ratio) {
    aspectRatio = ratio;
}

void Kamera::addShake(const float intensity, const float duration) {
    shake.intensity = intensity;
    shake.duration = duration;
    shake.timeLeft = duration;
}

void Kamera::updateShake(const float dt) {
    if (shake.timeLeft <= 0.0f) {
        shake.offset = glm::vec3(0.0f);
        return;
    }

    shake.timeLeft -= dt;

    const float t = shake.timeLeft / shake.duration;
    const float current = shake.intensity * glm::smoothstep(0.0f, 1.0f, t);

    shake.offset.x = Random::range(-1.0f, 1.0f) * current;
    shake.offset.y = Random::range(-1.0f, 1.0f) * current;
    shake.offset.z = Random::range(-0.5f, 0.5f) * current;
}

float Kamera::getAspectRatio() const {
    return aspectRatio;
}
