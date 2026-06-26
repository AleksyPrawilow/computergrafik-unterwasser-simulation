//
// Created by Kajetan on 26/06/2026.
//

#include "fadeOverlay.h"
#include "werkzeuge/textur.h"

void FadeOverlay::onInit() {
    material.albedo = Kern::LoadTexture("assets/textures/schwarz.png", true);
    visible = false;
}

void FadeOverlay::fadeIn(float dauer, std::function<void()> callback) {
    zielAlpha = 1.0f;
    geschwindigkeit = 1.0f / glm::max(dauer, 0.01f);
    fertigCallback = std::move(callback);
    visible = true;
}

void FadeOverlay::fadeOut(float dauer, std::function<void()> callback) {
    zielAlpha = 0.0f;
    geschwindigkeit = 1.0f / glm::max(dauer, 0.01f);
    fertigCallback = std::move(callback);
    visible = true;
}

void FadeOverlay::sofort(float a) {
    alpha = a;
    zielAlpha = a;
    geschwindigkeit = 0.0f;
    visible = (a > 0.001f);
}

void FadeOverlay::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (glm::abs(alpha - zielAlpha) < 0.001f) {
        alpha = zielAlpha;
        if (alpha < 0.001f) visible = false;
        if (fertigCallback) {
            auto cb = std::move(fertigCallback);
            fertigCallback = nullptr;
            cb();
        }
        return;
    }

    if (alpha < zielAlpha) {
        alpha = glm::min(alpha + geschwindigkeit * deltaTime, zielAlpha);
    } else {
        alpha = glm::max(alpha - geschwindigkeit * deltaTime, zielAlpha);
    }
}

void FadeOverlay::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (!visible || alpha < 0.001f || material.albedo == 0) return;

    glUseProgram(material.shader);

    const glm::mat4 ortho = Kern::GetOrthoProjection();
    const glm::vec2 viewport = Kern::GetViewportSize();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(viewport.x, viewport.y, 1.0f));

    Kern::setUniform(material.shader, "ortho", ortho);
    Kern::setUniform(material.shader, "model", model);
    Kern::SetActiveTexture(material.albedo, "uiTexture", material.shader, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendColor(0.0f, 0.0f, 0.0f, alpha);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

    Kern::DrawQuad(sharedVAO);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(0);
}
