//
// Created by Alexey Pravilov on 02/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CROSSHAIR_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CROSSHAIR_H
#include "glew.h"
#include "glm.hpp"
#include "werkzeuge/transform.h"

class Fadenkreuz {
public:
    Fadenkreuz();
    ~Fadenkreuz();
    void init(float aspectRatio);
    void draw(const Transform& target, const glm::mat4& viewProj) const;

private:
    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
};
#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CROSSHAIR_H
