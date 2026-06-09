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
    void drawOpaque(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) const;
    void drawTransparent(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void drawUI(const glm::mat4& view, const glm::mat4& projection) const;
    void clearQueues() {
        opaqueQueue.clear();
        transparentQueue.clear();
        uiQueue.clear();
    }

private:
    void drawElement(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) const;

    std::vector<const Wesen*> opaqueQueue;
    std::vector<const Wesen*> transparentQueue;
    std::vector<const Wesen*> uiQueue;

    GLuint defaultNormal = 0;
    GLuint defaultEmission = 0;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
