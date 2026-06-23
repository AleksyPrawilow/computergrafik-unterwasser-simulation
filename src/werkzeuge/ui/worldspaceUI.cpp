//
// Created by Alexey Pravilov on 09/06/2026.
//

#include "worldspaceUI.h"

#include "werkzeuge/kamera.h"
#include "werkzeuge/textur.h"

extern Kamera kamera;

void WorldspaceUI::setTarget(Wesen* target, const glm::vec3& offset) {
    targetEntity = target;
    worldOffset = offset;
}

void WorldspaceUI::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!enabled || targetEntity == nullptr || targetEntity->isQueuedDestroyed) {
        visible = false;
        return;
    }

    if (baseScale.x == 0.0f) {
        baseScale = glm::vec2(transform.scale.x, transform.scale.y);
    }

    const glm::vec3 worldPos = targetEntity->getGlobalTransform().position + worldOffset;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const glm::vec4 vpVector(viewport[0], viewport[1], viewport[2], viewport[3]);

    const glm::mat4 view = kamera.getViewMatrix();
    const glm::mat4 projection = kamera.getProjectionMatrix();

    if (const glm::vec3 screenPos = glm::project(worldPos, view, projection, vpVector); screenPos.z < 0.0f || screenPos.z > 1.0f) {
        visible = false;
    } else {
        visible = true;

        float scaleFactor = 1.0f;
        if (shouldScale) {
            float dist = glm::distance(worldPos, kamera.transform.position);
            float referenceDist = 12.0f;
            scaleFactor = referenceDist / dist;
            scaleFactor = glm::clamp(scaleFactor, 0.4f, 1.4f);
        }

        transform.scale = glm::vec3(baseScale * scaleFactor, 1.0f);

        transform.position = glm::vec3(
            screenPos.x,
            static_cast<float>(viewport[3]) - screenPos.y,
            0.0f
        );
    }
}

float WorldspaceUI::getUIScaleFactor() const {
    if (!shouldScale || baseScale.x == 0.0f) return 1.0f;
    return transform.scale.x / baseScale.x;
}
