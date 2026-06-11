//
// Created by Alexey Pravilov on 02/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CROSSHAIR_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CROSSHAIR_H
#include "glew.h"
#include "werkzeuge/ui/wesenUI.h"

class Fadenkreuz: public UIElement {
public:
    Fadenkreuz();
    ~Fadenkreuz() override;

    void init(float aspectRatio);

    // --- ENGINES-LEVEL OVERRIDES ---
    bool hasCustomRender() const override { return true; }
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;

private:
    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
};
#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CROSSHAIR_H
