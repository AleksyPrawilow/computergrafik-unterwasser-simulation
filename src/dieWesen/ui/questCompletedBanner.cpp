//
// Created by Alexey Pravilov on 20/06/2026.
//

#include "questCompletedBanner.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"

class DissolvingLabel : public UILabel {
public:
    const float* dissolveRef = nullptr;
    GLuint shader = 0;

    void onInit() override {
        UILabel::onInit();
        material.shader = shader;
    }

    void customRender(const glm::mat4& view, const glm::mat4& projection) const override {
        if (!visible || material.albedo == 0 || text.empty()) return;

        glUseProgram(material.shader);
        Kern::setUniform(material.shader, "u_dissolve", dissolveRef ? *dissolveRef : 0.0f);

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

    topBorder = new DissolvingLabel();
    topBorder->dissolveRef = &topDissolve;
    topBorder->shader = dissolveShader;
    topBorder->setText(flourishLine, 24.0f);
    topBorder->color = glm::vec4(0.85f, 0.65f, 0.15f, 1.0f);
    addChild(topBorder);

    titleLabel = new DissolvingLabel();
    titleLabel->dissolveRef = &titleDissolve;
    titleLabel->shader = dissolveShader;
    titleLabel->setText("Q U E S T   C O M P L E T E D", 64.0f);
    titleLabel->color = glm::vec4(1.0f, 0.85f, 0.2f, 1.0f);
    addChild(titleLabel);

    subLabel = new UILabel();
    subLabel->setText(questTitle, 0.0f);
    subLabel->color = glm::vec4(0.9f, 0.95f, 1.0f, 0.0f);
    addChild(subLabel);

    bottomBorder = new DissolvingLabel();
    bottomBorder->dissolveRef = &bottomDissolve;
    bottomBorder->shader = dissolveShader;
    bottomBorder->setText(flourishLine, 24.0f);
    bottomBorder->color = glm::vec4(0.85f, 0.65f, 0.15f, 1.0f);
    addChild(bottomBorder);

    glm::vec2 size = Kern::GetViewportSize();
    transform.position = glm::vec3(size.x / 2.0f, size.y / 2.0f, 0.0f);

    createTween()
        ->tweenProperty(&topDissolve, 0.0f, 0.25f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&bottomDissolve, 0.0f, 0.25f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&titleDissolve, 0.0f, 0.7f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&subtitleFontSize, 32.0f, 0.35f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&subtitleAlpha, 1.0f, 0.25f, EaseType::EASE_OUT_SINE)
        ->tweenInterval(2.2f)
        ->tweenProperty(&topDissolve, 1.0f, 2.5f, EaseType::EASE_IN_SINE)
        ->parallel()
        ->tweenProperty(&titleDissolve, 1.0f, 2.5f, EaseType::EASE_IN_SINE)
        ->parallel()
        ->tweenProperty(&bottomDissolve, 1.0f, 2.5f, EaseType::EASE_IN_SINE)
        ->parallel()
        ->tweenProperty(&subtitleAlpha, 0.0f, 1.5f, EaseType::EASE_IN_SINE)
        ->parallel()
        ->tweenProperty(&subtitleFontSize, 0.0f, 1.55f, EaseType::EASE_OUT_BACK)
        ->tweenCallback([this]() {
            this->queueDestroy();
        });
}

void QuestCompletedBanner::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    glm::vec2 size = Kern::GetViewportSize();
    transform.position = glm::vec3(size.x / 2.0f, size.y / 2.0f, 0.0f);
    subLabel->fontSize = subtitleFontSize;
    subLabel->color.w = subtitleAlpha;
    VBoxUI::onUpdate(window, deltaTime, cameraTransform);
}