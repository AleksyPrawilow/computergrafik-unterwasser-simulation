//
// Created by Alexey Pravilov on 26/06/2026.
//

#include "cinematicBars.h"
#include "werkzeuge/textur.h"

void CinematicBars::onInit() {
    material.albedo = Kern::LoadTexture("assets/textures/default_emission.png");
    addToGroup("CinematicBars");
}

void CinematicBars::setEnabled(bool enabled) {
    targetProgress = enabled ? 1.0f : 0.0f;
}

void CinematicBars::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    progress = glm::mix(progress, targetProgress, 1.0f - glm::exp(-transitionSpeed * deltaTime));
    visible = (progress > 0.001f);
}

void CinematicBars::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (!visible || material.albedo == 0) return;

    glUseProgram(material.shader);

    const glm::mat4 ortho = Kern::GetOrthoProjection();
    const glm::vec2 viewport = Kern::GetViewportSize();

    const float targetHeight = viewport.y * heightPercentage;
    const float currentHeight = targetHeight * progress;

    {
        glm::vec2 pos(0.0f, viewport.y - currentHeight);

        // NOTE: If your UI coordinate system is Y-down (0 is top of screen), use:
        // glm::vec2 pos(0.0f, 0.0f);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
        model = glm::scale(model, glm::vec3(viewport.x, currentHeight, 1.0f));

        Kern::setUniform(material.shader, "ortho", ortho);
        Kern::setUniform(material.shader, "model", model);
        Kern::SetActiveTexture(material.albedo, "uiTexture", material.shader, 0);
        Kern::DrawQuad(sharedVAO);
    }

    {
        // For Y-up coordinate systems (0 is bottom of screen)
        glm::vec2 pos(0.0f, 0.0f);

        // NOTE: If your UI coordinate system is Y-down (0 is top of screen), use:
        // glm::vec2 pos(0.0f, viewport.y - currentHeight);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
        model = glm::scale(model, glm::vec3(viewport.x, currentHeight, 1.0f));

        Kern::setUniform(material.shader, "ortho", ortho);
        Kern::setUniform(material.shader, "model", model);
        Kern::SetActiveTexture(material.albedo, "uiTexture", material.shader, 0);
        Kern::DrawQuad(sharedVAO);
    }

    glUseProgram(0);
}