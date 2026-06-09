//
// Created by Alexey Pravilov on 01/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
#include "wesen.h"
#include <glm.hpp>
#include <vector>

class Wesen;

class Renderer {
public:
    void init();
    void render(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void drawOpaque(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void drawTransparent(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void clearQueues() {
        opaqueQueue.clear();
        transparentQueue.clear();
    }

private:
    void drawElement(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);

    std::vector<const Wesen*> opaqueQueue;
    std::vector<const Wesen*> transparentQueue;

    GLuint defaultNormal = 0;
    GLuint defaultEmission = 0;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
