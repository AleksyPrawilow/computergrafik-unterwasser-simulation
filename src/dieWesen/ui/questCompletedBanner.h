//
// Created by Alexey Pravilov on 20/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTCOMPLETEDBANNER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTCOMPLETEDBANNER_H
#include "werkzeuge/ui/uiContainers.h"
#include "werkzeuge/ui/uiLabel.h"

class DissolvingLabel;

class QuestCompletedBanner : public VBoxUI {
public:
    explicit QuestCompletedBanner(const std::string& questName, const std::string& titleOverride = "");

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    // Independent transition parameters for each text element
    float topDissolve = 1.0f;
    float titleDissolve = 1.0f;
    float bottomDissolve = 1.0f;
    float subtitleFontSize = 0.0f;
    float subtitleAlpha = 0.0f;

private:
    std::string questTitle;
    std::string titelText;
    static GLuint dissolveShader;

    DissolvingLabel* topBorder = nullptr;
    DissolvingLabel* titleLabel = nullptr;
    UILabel* subLabel = nullptr;
    DissolvingLabel* bottomBorder = nullptr;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTCOMPLETEDBANNER_H
