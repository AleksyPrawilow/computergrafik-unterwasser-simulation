//
// Created by Alexey Pravilov on 08/06/2026.
//

#include "oceanFloor.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void OceanFloor::init() {
    material.albedo = Kern::LoadTexture("assets/textures/ocean-rock_albedo.png");
    material.normal = Kern::LoadTexture("assets/textures/ocean-rock_normal-ogl.png");
    material.roughness = Kern::LoadTexture("assets/textures/ocean-rock_roughness.png");
    transform.position = glm::vec3(0.0f, -285.0f, 0.0f);
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    loadModel("assets/models/oceanbed.obj", &vertices);
    transform.scale = glm::vec3(1.0f);
    addToGroup("OceanFloor");
}

float OceanFloor::getHeight(const float x, const float z) const {
    float closestDistanceSq = std::numeric_limits<float>::max();
    float groundHeight = -1.0f;
    auto [position, rotation, scale] = getGlobalTransform();

    for (const auto& localPos : vertices) {
        const glm::vec3 worldPos = position + (rotation * (localPos * scale));

        const float dx = worldPos.x - x;
        const float dz = worldPos.z - z;

        if (const float distSq = dx * dx + dz * dz; distSq < closestDistanceSq) {
            closestDistanceSq = distSq;
            groundHeight = worldPos.y;
        }
    }

    return groundHeight;
}