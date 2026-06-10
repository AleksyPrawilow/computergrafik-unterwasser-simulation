//
// Created by Alexey Pravilov on 09/06/2026.
//

#include "wesenUI.h"

#include "werkzeuge/textur.h"

void UIElement::init() {
    material.shader = ShaderManager::getInstance().loadShader(
        "ui",
        "assets/shaders/ui.vert",
        "assets/shaders/ui.frag"
        );

    constexpr float vertices[] = {
        0.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        1.0f, 1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    material.isTransparent = true;
    material.isUI = true;

    onInit();
}

void UIElement::onInit() {}

void UIElement::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (!visible || material.albedo == 0) return;

    glUseProgram(material.shader);

    glm::mat4 ortho = Kern::GetOrthoProjection();

    const Transform global = getGlobalTransform();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(global.position.x, global.position.y, 0.0f));
    model = glm::scale(model, glm::vec3(global.scale.x, global.scale.y, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(material.shader, "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
    glUniformMatrix4fv(glGetUniformLocation(material.shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    Kern::SetActiveTexture(material.albedo, "uiTexture", material.shader, 0);

    Kern::DrawQuad(vao);

    glUseProgram(0);
}

void UIElement::updateGlobalTransforms() {
    if (parent == nullptr) {
        cachedGlobalModelMatrix = transform.getModelMatrix();
        cachedGlobalTransform = transform;
        return;
    }

    const Transform parentGlobal = parent->getGlobalTransform();

    cachedGlobalTransform.scale = transform.scale;
    cachedGlobalTransform.rotation = parentGlobal.rotation * transform.rotation;
    cachedGlobalTransform.position = parentGlobal.position + (parentGlobal.rotation * transform.position);

    const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cachedGlobalTransform.position.x, cachedGlobalTransform.position.y, 0.0f));
    cachedGlobalModelMatrix = glm::scale(model, glm::vec3(cachedGlobalTransform.scale.x, cachedGlobalTransform.scale.y, 1.0f));
}
