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
    buildGrid();
}

void OceanFloor::buildGrid() {
    grid.clear();
    for (int i = 0; i < static_cast<int>(vertices.size()); i++) {
        int gx = static_cast<int>(glm::floor(vertices[i].x / gridCellSize));
        int gz = static_cast<int>(glm::floor(vertices[i].z / gridCellSize));
        grid[gridKey(gx, gz)].push_back(i);
    }
}

float OceanFloor::getHeight(const float x, const float z) const {
    glm::vec3 localQuery = (glm::vec3(x, 0.0f, z) - transform.position) / transform.scale;

    int gx = static_cast<int>(glm::floor(localQuery.x / gridCellSize));
    int gz = static_cast<int>(glm::floor(localQuery.z / gridCellSize));

    float closestDistSq = std::numeric_limits<float>::max();
    float groundHeight = -1.0f;

    for (int dx = -2; dx <= 2; dx++) {
        for (int dz = -2; dz <= 2; dz++) {
            auto it = grid.find(gridKey(gx + dx, gz + dz));
            if (it == grid.end()) continue;
            for (int idx : it->second) {
                float ddx = vertices[idx].x - localQuery.x;
                float ddz = vertices[idx].z - localQuery.z;
                float distSq = ddx * ddx + ddz * ddz;
                if (distSq < closestDistSq) {
                    closestDistSq = distSq;
                    groundHeight = transform.position.y + vertices[idx].y * transform.scale.y;
                }
            }
        }
    }

    return groundHeight;
}
