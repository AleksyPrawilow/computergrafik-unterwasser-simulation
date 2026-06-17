//
// Created by Alexey Pravilov on 09/06/2026.
//

#include "uiLabel.h"

#include "werkzeuge/textur.h"

void UILabel::onInit() {
    material.albedo = sharedFontTexture;
}

void UILabel::setText(const std::string& text, const float size) {
    this->text = text;
    this->fontSize = size;
    this->material.albedo = sharedFontTexture;
    this->transform.scale = glm::vec3(static_cast<float>(text.length()) * (fontSize * 0.6f), fontSize, 1.0f);
}

void UILabel::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    this->transform.scale.x = static_cast<float>(text.length()) * (fontSize * 0.6f);
    this->transform.scale.y = fontSize;
}

void UILabel::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (!visible || material.albedo == 0 || text.empty()) return;

    glUseProgram(material.shader);

    glm::mat4 ortho = Kern::GetOrthoProjection();

    Kern::setUniform(material.shader, "isText", 1);
    Kern::setUniform(material.shader, "textColor", color);

    Kern::SetActiveTexture(material.albedo, "uiTexture", material.shader, 0);

    float currentScaleFactor = getUIScaleFactor();
    float scaledFontSize = fontSize * currentScaleFactor;
    float charSpacing = scaledFontSize * 0.6f;
    const Transform global = getGlobalTransform();

    glBindVertexArray(sharedVAO);

    for (size_t i = 0; i < text.length(); ++i) {
        const char c = text[i];
        const int col = c % 16;
        const int row = c / 16;
        auto uvOffset = glm::vec2(static_cast<float>(col) / 16.0f, static_cast<float>(row) / 16.0f);

        Kern::setUniform(material.shader, "uvOffset", uvOffset);

        auto charPos = glm::vec2(global.position.x + static_cast<float>(i) * charSpacing, global.position.y);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(charPos, 0.0f));
        model = glm::scale(model, glm::vec3(scaledFontSize, scaledFontSize, 1.0f));

        Kern::setUniform(material.shader, "model", model);
        Kern::setUniform(material.shader, "ortho", ortho);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindVertexArray(0);

    Kern::setUniform(material.shader, "isText", 0);
    glUseProgram(0);
}
