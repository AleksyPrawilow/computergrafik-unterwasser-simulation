//
// Created by Alexey Pravilov on 09/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WORLDSPACEUI_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WORLDSPACEUI_H
#include "wesenUI.h"


class WorldspaceUI : public UIElement {
public:
    Wesen* targetEntity = nullptr;
    glm::vec3 worldOffset = glm::vec3(0.0f);

    bool shouldScale = true;
    bool enabled = true;

    void setTarget(Wesen* target, const glm::vec3& offset = glm::vec3(0.0f));
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    float getUIScaleFactor() const override;

private:
    glm::vec2 baseScale = glm::vec2(0.0f);
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WORLDSPACEUI_H
