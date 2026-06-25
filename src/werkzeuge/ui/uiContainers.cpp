//
// Created by Alexey Pravilov on 10/06/2026.
//

#include "uiContainers.h"

VBoxUI* VBoxUI::setAlignment(UIAlignment align) {
    this->alignment = align;
    return this;
}

void VBoxUI::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    float totalHeight = 0.0f;
    float maxWidth = 0.0f;

    for (const Wesen* child : children) {
        if (!child->visible) continue;
        maxWidth = glm::max(maxWidth, child->transform.scale.x);
        totalHeight += child->transform.scale.y + spacing;
    }

    totalHeight = glm::max(totalHeight - spacing, 0.0f);
    transform.scale = glm::vec3(maxWidth, totalHeight, 1.0f);

    float currentY = 0.0f;
    for (Wesen* child : children) {
        if (!child->visible) continue;
        float localX = 0.0f;

        if (alignment == UIAlignment::CENTER) {
            localX = (maxWidth - child->transform.scale.x) / 2.0f;
        }
        else if (alignment == UIAlignment::END) {
            localX = maxWidth - child->transform.scale.x;
        }

        child->transform.position = glm::vec3(localX, currentY, 0.0f);
        currentY += child->transform.scale.y + spacing;
    }
}

HBoxUI* HBoxUI::setAlignment(UIAlignment align) {
    this->alignment = align;
    return this;
}

void HBoxUI::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    float totalWidth = 0.0f;
    float maxHeight = 0.0f;

    for (const Wesen* child : children) {
        maxHeight = glm::max(maxHeight, child->transform.scale.y);
        totalWidth += child->transform.scale.x + spacing;
    }

    totalWidth = glm::max(totalWidth - spacing, 0.0f);
    transform.scale = glm::vec3(totalWidth, maxHeight, 1.0f);

    float currentX = 0.0f;
    for (Wesen* child : children) {
        float localY = 0.0f;

        if (alignment == UIAlignment::CENTER) {
            localY = (maxHeight - child->transform.scale.y) / 2.0f;
        }
        else if (alignment == UIAlignment::END) {
            localY = maxHeight - child->transform.scale.y;
        }

        child->transform.position = glm::vec3(currentX, localY, 0.0f);
        currentX += child->transform.scale.x + spacing;
    }
}
