//
// Created by Alexey Pravilov on 09/06/2026.
//

#include "worldspaceUI.h"
#include "werkzeuge/textur.h"

void WorldspaceUI::setTarget(Wesen* target, const glm::vec3& offset) {
    targetEntity = target;
    worldOffset = offset;
}

void WorldspaceUI::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    if (targetEntity == nullptr || targetEntity->isQueuedDestroyed || !visible || material.albedo == 0) return;

    const glm::vec3 worldPos = targetEntity->getGlobalTransform().position + worldOffset;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const glm::vec4 vpVector(viewport[0], viewport[1], viewport[2], viewport[3]);
    const glm::vec3 screenPos = glm::project(worldPos, view, projection, vpVector);

    if (screenPos.z < 0.0f || screenPos.z > 1.0f) return;

    const glm::vec2 size = transform.scale;
    const auto screenPosition = glm::vec2(screenPos.x - size.x / 2.0f, (viewport[3] - screenPos.y) - size.y / 2.0f);

    glUseProgram(material.shader);
    glm::mat4 ortho = Kern::GetOrthoProjection();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(screenPosition, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(material.shader, "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
    glUniformMatrix4fv(glGetUniformLocation(material.shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    Kern::SetActiveTexture(material.albedo, "uiTexture", material.shader, 0);


    Kern::DrawQuad(vao);

    glUseProgram(0);
}
