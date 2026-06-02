//
// Created by Alexey Pravilov on 01/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
#include "daswesen.h"


class Renderer {
public:
    void render(const Daswesen& e, const glm::mat4& viewProj, const glm::vec3& cameraPos);
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
