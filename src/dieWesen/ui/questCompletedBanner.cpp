//
// Created by Alexey Pravilov on 20/06/2026.
//

#include "questCompletedBanner.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"
#include <iostream>

// Specialized internal label that safely overrides the engine's default UI shader
class DissolvingLabel : public UILabel {
public:
    const float* dissolveRef = nullptr;
    GLuint shader = 0;

    void onInit() override {
        UILabel::onInit();
        // Override the sharedShader applied by UIElement::init()
        material.shader = shader;
    }

    void customRender(const glm::mat4& view, const glm::mat4& projection) const override {
        if (!visible || material.albedo == 0 || text.empty()) return;

        // Force-bind our custom shader program and set the uniform
        glUseProgram(material.shader);
        Kern::setUniform(material.shader, "u_dissolve", dissolveRef ? *dissolveRef : 0.0f);

        // Run standard UILabel text rendering (which now successfully binds the dissolve shader)
        UILabel::customRender(view, projection);
    }
};

GLuint QuestCompletedBanner::dissolveShader = 0;

QuestCompletedBanner::QuestCompletedBanner(const std::string& questName)
    : VBoxUI(12.0f), questTitle(questName) {}

void QuestCompletedBanner::init() {
    if (dissolveShader == 0) {
        dissolveShader = ShaderManager::getInstance().loadShader(
            "quest_dissolve",
            "assets/shaders/ui.vert",
            "assets/shaders/quest_dissolve.frag"
        );
    }

    setAlignment(UIAlignment::CENTER);
    setExpansion(UIExpansion::CENTER);

    const std::string flourishLine = "======================================";

    // 1. Top Border
    topBorder = new DissolvingLabel();
    topBorder->dissolveRef = &topDissolve;
    topBorder->shader = dissolveShader;
    topBorder->setText(flourishLine, 24.0f);
    topBorder->color = glm::vec4(0.85f, 0.65f, 0.15f, 1.0f);
    addChild(topBorder); // Calls topBorder->init() -> topBorder->onInit()

    // 2. Main Title
    titleLabel = new DissolvingLabel();
    titleLabel->dissolveRef = &titleDissolve;
    titleLabel->shader = dissolveShader;
    titleLabel->setText("Q U E S T   C O M P L E T E D", 64.0f);
    titleLabel->color = glm::vec4(1.0f, 0.85f, 0.2f, 1.0f);
    addChild(titleLabel);

    // 3. Subtitle (Standard UILabel with scale pop)
    subLabel = new UILabel();
    subLabel->setText(questTitle, 0.0f);
    subLabel->color = glm::vec4(0.9f, 0.95f, 1.0f, 0.0f);
    addChild(subLabel);

    // 4. Bottom Border
    bottomBorder = new DissolvingLabel();
    bottomBorder->dissolveRef = &bottomDissolve;
    bottomBorder->shader = dissolveShader;
    bottomBorder->setText(flourishLine, 24.0f);
    bottomBorder->color = glm::vec4(0.85f, 0.65f, 0.15f, 1.0f);
    addChild(bottomBorder);

    // Position the pivot origin exactly to screen-center
    glm::vec2 size = Kern::GetViewportSize();
    transform.position = glm::vec3(size.x / 2.0f, size.y / 2.0f, 0.0f);

    // Choreographing the dynamic cascading entrance and exit
    createTween()
        // Step 1: Top border dissolves into view (1.0 -> 0.0)
        ->tweenProperty(&topDissolve, 0.0f, 0.25f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&bottomDissolve, 0.0f, 0.25f, EaseType::EASE_OUT_SINE)

        // Step 2: Main title dissolves in right after
        ->tweenProperty(&titleDissolve, 0.0f, 0.7f, EaseType::EASE_OUT_SINE)

        // Step 4: Subtitle elastically pops and fades in last
        ->tweenProperty(&subtitleFontSize, 32.0f, 0.35f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&subtitleAlpha, 1.0f, 0.25f, EaseType::EASE_OUT_SINE)

        // Step 5: Hold everything fully visible on screen
        ->tweenInterval(2.2f)

        // Step 6: Dissolve, shrink, and fade everything out together
        ->tweenProperty(&topDissolve, 1.0f, 2.5f, EaseType::EASE_IN_SINE)
        ->parallel()
        ->tweenProperty(&titleDissolve, 1.0f, 2.5f, EaseType::EASE_IN_SINE)
        ->parallel()
        ->tweenProperty(&bottomDissolve, 1.0f, 2.5f, EaseType::EASE_IN_SINE)
        ->parallel()
        ->tweenProperty(&subtitleAlpha, 0.0f, 1.5f, EaseType::EASE_IN_SINE)
        ->parallel()
        ->tweenProperty(&subtitleFontSize, 0.0f, 1.55f, EaseType::EASE_OUT_BACK)

        // Step 7: Delete the container node from the scene graph
        ->tweenCallback([this]() {
            this->queueDestroy();
        });
}

void QuestCompletedBanner::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    // Keep container pinned to screen center during window resizing
    glm::vec2 size = Kern::GetViewportSize();
    transform.position = glm::vec3(size.x / 2.0f, size.y / 2.0f, 0.0f);

    // Apply subtitle update parameters
    subLabel->fontSize = subtitleFontSize;
    subLabel->color.w = subtitleAlpha;

    // Call base VBoxUI update to arrange child elements vertically
    VBoxUI::onUpdate(window, deltaTime, cameraTransform);
}