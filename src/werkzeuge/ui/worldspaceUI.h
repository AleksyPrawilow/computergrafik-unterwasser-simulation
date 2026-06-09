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

    void setTarget(Wesen* target, const glm::vec3& offset = glm::vec3(0.0f));
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WORLDSPACEUI_H
