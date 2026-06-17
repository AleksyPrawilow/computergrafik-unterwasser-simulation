//
// Created by Alexey Pravilov on 09/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WESENUI_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WESENUI_H
#include "../wesen.h"
#include "../shaderManager.h"
#include "gtc/type_ptr.hpp"

enum class UIExpansion {
    RIGHT,
    CENTER,
    LEFT
};

class UIElement : public Wesen {
public:
    bool visible = true;
    UIExpansion expansion = UIExpansion::RIGHT;

    static GLuint sharedFontTexture;
    static GLuint sharedVAO;
    static GLuint sharedVBO;
    static GLuint sharedShader;

    static void initUISystem();
    static void cleanupUISystem();
    void init() override;
    virtual void onInit();

    ~UIElement() override = default;

    UIElement* setExpansion(UIExpansion exp);

    [[nodiscard]] bool hasCustomRender() const override { return true; }
    Transform getGlobalTransform() const override;
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;

protected:
    void updateGlobalTransforms() override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WESENUI_H
