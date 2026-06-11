//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "fadenkreuz.h"
#include "werkzeuge/transform.h"

Fadenkreuz::Fadenkreuz() = default;

Fadenkreuz::~Fadenkreuz() {
    if (shaderProgram != 0) glDeleteProgram(shaderProgram);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void Fadenkreuz::init(const float aspectRatio) {
    UIElement::init();

    shaderProgram = ShaderManager::getInstance().loadShader(
        "fadenkreuz",
        "assets/shaders/fadenkreuz.vert",
        "assets/shaders/fadenkreuz.frag"
    );

    material.shader = shaderProgram;

    constexpr float wSize = 0.020f;
    constexpr float hGap = 0.006f;
    constexpr float dotSize = 0.002f;

    const float vertices[] = {
        -wSize - hGap, 0.0f,    -hGap, 0.0f,
         hGap, 0.0f,             wSize + hGap, 0.0f,
        -dotSize, 0.0f,          dotSize, 0.0f,
         0.0f, -dotSize * aspectRatio,  0.0f, dotSize * aspectRatio
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Fadenkreuz::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (parent == nullptr || !visible) return;

    Transform parentGlobal = parent->getGlobalTransform();

    constexpr float aimDistance = 20.0f;
    glm::vec3 worldAimPos = parentGlobal.position + (parentGlobal.rotation * glm::vec3(0.0f, 0.0f, -aimDistance));

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const glm::vec4 vpVector(viewport[0], viewport[1], viewport[2], viewport[3]);
    const glm::vec3 screenPos = glm::project(worldAimPos, view, projection, vpVector);

    if (screenPos.z < 0.0f || screenPos.z > 1.0f) return;

    // Convert screen pixels back to NDC [-1.0, 1.0]
    glm::vec2 ndcOffset;
    ndcOffset.x = (2.0f * screenPos.x) / vpVector[2] - 1.0f;

    // --- FIXED: Removed the vertical flip. NDC and glm::project already match! ---
    ndcOffset.y = (2.0f * screenPos.y) / vpVector[3] - 1.0f;

    glUseProgram(shaderProgram);
    glUniform2f(glGetUniformLocation(shaderProgram, "u_Offset"), ndcOffset.x, ndcOffset.y);

    glBindVertexArray(VAO);

    // --- CLEANED UP: No raw depth testing modifications here anymore ---
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 8);

    glBindVertexArray(0);
    glUseProgram(0);
}