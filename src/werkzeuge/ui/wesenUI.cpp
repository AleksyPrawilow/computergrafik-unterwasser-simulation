//
// Created by Alexey Pravilov on 09/06/2026.
//

#include "wesenUI.h"

#include "werkzeuge/textur.h"

GLuint UIElement::sharedFontTexture = 0;
GLuint UIElement::sharedVAO = 0;
GLuint UIElement::sharedVBO = 0;
GLuint UIElement::sharedShader = 0;

void UIElement::initUISystem() {
    if (sharedShader != 0) return;

    sharedShader = ShaderManager::getInstance().loadShader(
        "ui",
        "assets/shaders/ui.vert",
        "assets/shaders/ui.frag"
    );

    sharedFontTexture = Kern::LoadTexture("assets/textures/font_atlas.png", true);

    constexpr float vertices[] = {
        0.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        1.0f, 1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &sharedVAO);
    glGenBuffers(1, &sharedVBO);
    glBindVertexArray(sharedVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glBindVertexArray(0);

    std::cout<< "Loaded UI";
}

void UIElement::cleanupUISystem() {
    if (sharedVAO != 0) glDeleteVertexArrays(1, &sharedVAO);
    if (sharedVBO != 0) glDeleteBuffers(1, &sharedVBO);
    sharedVAO = 0;
    sharedVBO = 0;
    sharedShader = 0;
    sharedFontTexture = 0;
}

void UIElement::init() {
    material.shader = sharedShader;
    material.isTransparent = true;
    material.isUI = true;

    onInit();
}

void UIElement::onInit() {}

UIElement* UIElement::setExpansion(UIExpansion exp) {
    this->expansion = exp;
    return this;
}

Transform UIElement::getGlobalTransform() const {
    if (parent == nullptr) {
        Transform t = transform;

        // --- UPDATED: Centers both X and Y automatically! ---
        if (expansion == UIExpansion::CENTER) {
            t.position.x -= transform.scale.x / 2.0f;
            t.position.y -= transform.scale.y / 2.0f;
        } else if (expansion == UIExpansion::LEFT) {
            t.position.x -= transform.scale.x;
        }
        return t;
    }

    Transform parentGlobal = parent->getGlobalTransform();
    Transform global;

    float parentScaleFactor = parent->getUIScaleFactor();
    global.scale = transform.scale * parentScaleFactor;
    global.rotation = parentGlobal.rotation * transform.rotation;

    // Apply local offsets relative to parent's top-left
    glm::vec3 localPos = transform.position;
    if (expansion == UIExpansion::CENTER) {
        localPos.x -= transform.scale.x / 2.0f;
        localPos.y -= transform.scale.y / 2.0f; // Centers both X and Y!
    } else if (expansion == UIExpansion::LEFT) {
        localPos.x -= transform.scale.x;
    }

    global.position = parentGlobal.position + (parentGlobal.rotation * (localPos * parentScaleFactor));
    return global;
}

void UIElement::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (!visible || material.albedo == 0) return;

    glUseProgram(material.shader);

    const glm::mat4 ortho = Kern::GetOrthoProjection();

    const Transform global = getGlobalTransform();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(global.position.x, global.position.y, 0.0f));
    model = glm::scale(model, glm::vec3(global.scale.x, global.scale.y, 1.0f));

    Kern::setUniform(material.shader, "ortho", ortho);
    Kern::setUniform(material.shader, "model", model);

    Kern::SetActiveTexture(material.albedo, "uiTexture", material.shader, 0);

    Kern::DrawQuad(sharedVAO);

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
