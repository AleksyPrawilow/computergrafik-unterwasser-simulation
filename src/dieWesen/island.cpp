//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "island.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "tree.h"

void Island::init() {
    loadModel("assets/models/island.obj", &vertices);
    material.albedo = Kern::LoadTexture("assets/textures/Ground_baseColor.png");
    material.normal = Kern::LoadTexture("assets/textures/Ground_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/Ground_metallicRoughness.png");
    material.roughness = Kern::LoadTexture("assets/textures/Ground_metallicRoughness.png");

    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );

    transform.position = glm::vec3(-700.0f, -10.0f, -220.0f);
    transform.scale = glm::vec3(4.0f, 4.0f, 4.0f);

    addToGroup("Island");
    addChild(new Tree());
    addChild(new Tree());
    addChild(new Tree());
    addChild(new Tree());
}

float Island::getHeight(const float x, const float z) const {
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
