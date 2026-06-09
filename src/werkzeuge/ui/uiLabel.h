//
// Created by Alexey Pravilov on 09/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UILABEL_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UILABEL_H
#include "wesenUI.h"


class UILabel : public UIElement {
public:
    std::string text;
    float fontSize = 16.0f;
    glm::vec4 color = glm::vec4(1.0f);

    void setText(const std::string& text, float size, GLuint fontTexture);
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UILABEL_H
