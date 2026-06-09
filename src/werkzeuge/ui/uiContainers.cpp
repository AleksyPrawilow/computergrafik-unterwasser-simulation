//
// Created by Alexey Pravilov on 10/06/2026.
//

#include "uiContainers.h"


void VBoxUI::setSpacing(const float newSpacing) {
    spacing = newSpacing;
    material.isTransparent = true;
}

void VBoxUI::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    float currentY = 0.0f;
    float maxWidth = 0.0f;

    for (Wesen* child : children) {
        child->transform.position = glm::vec3(0.0f, currentY, 0.0f);

        currentY += child->transform.scale.y + spacing;
        maxWidth = glm::max(maxWidth, child->transform.scale.x);
    }

    transform.scale = glm::vec3(maxWidth, glm::max(currentY - spacing, 0.0f), 1.0f);
}

void HBoxUI::setSpacing(const float newSpacing) {
    spacing = newSpacing;
    material.isTransparent = true;
}

void HBoxUI::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    float currentX = 0.0f;
    float maxHeight = 0.0f;

    for (Wesen* child : children) {
        child->transform.position = glm::vec3(currentX, 0.0f, 0.0f);

        currentX += child->transform.scale.x + spacing;
        maxHeight = glm::max(maxHeight, child->transform.scale.y);
    }

    transform.scale = glm::vec3(glm::max(currentX - spacing, 0.0f), maxHeight, 1.0f);
}
