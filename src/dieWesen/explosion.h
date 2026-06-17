#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_EXPLOSION_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_EXPLOSION_H

#include "../werkzeuge/wesen.h"

class Explosion : public Wesen {
public:
    explicit Explosion(const glm::vec3& position, float groesse = 3.0f);
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void prepareUniforms() const override;

private:
    glm::vec3 startPosition;
    float maxGroesse;
    float alter = 0.0f;
    float lebensdauer = 0.6f;
};

#endif
