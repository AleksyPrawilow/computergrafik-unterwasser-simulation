//
// Created by Alexey Pravilov on 20/06/2026.
//

#include "questHUD.h"
#include <sstream>
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/visual/questManager.h"
#include "werkzeuge/visual/tween.h"

QuestHUD::QuestHUD(float spacing) : VBoxUI(spacing) {}

void QuestHUD::onInit() {
    setAlignment(UIAlignment::END); // Aligns elements to the right inside the container
    setExpansion(UIExpansion::LEFT); // Shifts pivot so text scales out to the left

    // Position initial placement
    const glm::vec2 size = Kern::GetViewportSize();
    transform.position = glm::vec3(size.x - 20.0f, 20.0f, 0.0f);

    // Register UI update event
    QuestManager::getInstance().setOnProgressChangedCallback([this]() {
        this->refreshUI();
    });

    refreshUI();
}

void QuestHUD::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    const glm::vec2 size = Kern::GetViewportSize();
    transform.position.x = size.x - 20.0f;
    transform.position.y = 20.0f;

    VBoxUI::onUpdate(window, deltaTime, cameraTransform);
}

void QuestHUD::refreshUI() {
    for (Wesen* child : children) {
        child->queueDestroy();
    }
    children.clear();
    labelPool.clear();

    const auto& quests = QuestManager::getInstance().getActiveQuests();

    for (const auto& quest : quests) {
        if (quest.isCompleted) continue;

        // --- TITLE LABEL ---
        auto* titleLabel = new UILabel();
        // Initialize at size 0 and alpha 0 for the entry animation
        titleLabel->setText("[QUEST] " + quest.title, 0.0f);
        titleLabel->color = glm::vec4(1.0f, 0.85f, 0.2f, 0.0f);
        addChild(titleLabel);
        labelPool.push_back(titleLabel);

        titleLabel->createTween()
            ->tweenProperty(&titleLabel->fontSize, 48.0f, 0.45f, EaseType::EASE_OUT_BACK)
            ->parallel()
            ->tweenProperty(&titleLabel->color.w, 1.0f, 0.35f, EaseType::EASE_OUT_SINE);

        // --- OBJECTIVES LABELS ---
        for (const auto& obj : quest.objectives) {
            auto* objLabel = new UILabel();

            std::stringstream ss;
            ss << "  - " << obj.description;
            if (obj.requiredCount > 1) {
                ss << " (" << obj.currentCount << "/" << obj.requiredCount << ")";
            } else if (obj.isCompleted) {
                ss << " (DONE)";
            }

            // Initialize at size 0 and alpha 0
            objLabel->setText(ss.str(), 0.0f);

            float targetAlpha = obj.isCompleted ? 0.6f : 1.0f;
            glm::vec4 targetColor = obj.isCompleted
                ? glm::vec4(0.5f, 1.0f, 0.5f, 0.0f)   // Green
                : glm::vec4(0.9f, 0.9f, 0.9f, 0.0f);  // White

            objLabel->color = targetColor;
            addChild(objLabel);
            labelPool.push_back(objLabel);

            // Tween: Bounce-scale to 14px and fade in to target opacity
            objLabel->createTween()
                ->tweenProperty(&objLabel->fontSize, 32.0f, 0.45f, EaseType::EASE_OUT_BACK)
                ->parallel()
                ->tweenProperty(&objLabel->color.w, targetAlpha, 0.35f, EaseType::EASE_OUT_SINE);
        }
    }
}