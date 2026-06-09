//
// Created by Alexey Pravilov on 09/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WESENUI_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WESENUI_H
#include "../wesen.h"
#include "../shaderManager.h"
#include "gtc/type_ptr.hpp"

class UIElement : public Wesen {
public:
    bool visible = true;
    GLuint vao = 0;
    GLuint vbo = 0;

    void init() override;
    virtual void onInit();

    ~UIElement() override {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    [[nodiscard]] bool hasCustomRender() const override { return true; }
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;
    [[nodiscard]] Transform getGlobalTransform() const override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WESENUI_H
