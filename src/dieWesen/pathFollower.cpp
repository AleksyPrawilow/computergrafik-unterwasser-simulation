#include "pathFollower.h"
#include "gtc/matrix_transform.hpp"

void PathFollower::init() {
}

void PathFollower::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!targetPath || targetPath->frames.empty()) return;

    const auto& path = targetPath->frames;

    const PathFrame& targetFrame = path[currentNodeIndex];
    glm::vec3 targetPos = targetFrame.position;

    glm::vec3 direction = targetPos - transform.position;
    float distance = glm::length(direction);

    if (distance < 0.5f) {
        currentNodeIndex = (currentNodeIndex + 1) % path.size();
    } else {
        direction = glm::normalize(direction);
        transform.position += direction * moveSpeed * deltaTime;

        glm::vec3 pathForward = targetFrame.tangent;
        glm::vec3 pathUp = targetFrame.normal;
        
        transform.lookAt(transform.position + pathForward, pathUp);

        if (glm::abs(rotationOffsetY) > 0.001f) {
            transform.rotation = transform.rotation * glm::angleAxis(glm::radians(rotationOffsetY), glm::vec3(0, 1, 0));
        }
    }
}