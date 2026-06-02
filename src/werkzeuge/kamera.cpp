//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "kamera.h"

Kamera::Kamera() = default;
Kamera::~Kamera() = default;

glm::mat4 Kamera::getViewMatrix() const {
    const glm::mat4 R = glm::toMat4(glm::conjugate(transform.rotation));
    const glm::mat4 T = glm::translate(glm::mat4(1.0f), -transform.position);
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

float Kamera::getAspectRatio() const {
    return aspectRatio;
}