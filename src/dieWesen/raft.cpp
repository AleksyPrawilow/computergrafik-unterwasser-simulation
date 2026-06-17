//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "raft.h"
#include "uboot.h"
#include "werkzeuge/textur.h"

void Raft::init() {
    loadModel("assets/models/raft.obj");
    material.albedo = Kern::LoadTexture("assets/textures/default_opacity.png");
    material.opacity = Kern::LoadTexture("assets/textures/jellyfish_opacity.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.isTransparent = true;
    transform.position = glm::vec3(24.0f, 0.0f, 24.0f);
    transform.scale = glm::vec3(1.0f);
}

void Raft::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    const auto time = static_cast<float>(glfwGetTime());
    glm::vec3 pos = transform.position;

    auto rot = glm::mat3(transform.rotation);

    glm::vec3 corners[4] = {
        {-halfSize.x, 0, -halfSize.y},
        { halfSize.x, 0, -halfSize.y},
        { halfSize.x, 0,  halfSize.y},
        {-halfSize.x, 0,  halfSize.y}
    };

    float heights[4];
    glm::vec3 points[4];

    for (int i = 0; i < 4; i++) {
        points[i] = pos + rot * corners[i];
        heights[i] = Uboot::getWaterHeight(points[i].x, points[i].z, time);
    }

    const float avgHeight = (heights[0] + heights[1] + heights[2] + heights[3]) * 0.25f;

    glm::vec3 targetPos = pos;
    targetPos.y = avgHeight + 0.0f;

    transform.position = glm::mix(
        transform.position,
        targetPos,
        1.0f - exp(-heightSmooth * deltaTime)
    );

    // 4. compute tilt from wave slope
    float front = (heights[2] + heights[3]) * 0.5f;
    float back  = (heights[0] + heights[1]) * 0.5f;
    float right = (heights[1] + heights[2]) * 0.5f;
    float left  = (heights[0] + heights[3]) * 0.5f;

    float pitch = (back - front) * tiltStrength;
    float roll  = (left - right) * tiltStrength;

    glm::quat targetRot =
        glm::quat(glm::vec3(pitch, 0.0f, roll));

    // 5. smooth rotation
    transform.rotation = glm::slerp(
        transform.rotation,
        targetRot,
        1.0f - glm::exp(-rotSmooth * deltaTime)
    );
}