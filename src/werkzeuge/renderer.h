//
// Created by Alexey Pravilov on 01/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
#include "wesen.h"
#include <glm.hpp>

class Wesen;

class Renderer {
public:
    GLuint defaultNormal;
    GLuint defaultEmission;
    void init();
    void render(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
