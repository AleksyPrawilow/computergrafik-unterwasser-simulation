//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "island.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "tree.h"

void Island::init() {
    loadModel("assets/models/island.obj", &vertices);
    material.albedo = Kern::LoadTexture("assets/textures/sand_albedo.png");
    material.normal = Kern::LoadTexture("assets/textures/sand_normal.png");
    material.roughness = Kern::LoadTexture("assets/textures/sand_roughness.png");

    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );


    transform.position = glm::vec3(-700.0f, -10.0f, -220.0f);
    transform.scale = glm::vec3(6.0f, 6.0f, 6.0f);

    addToGroup("Island");

    auto* baum1 = new Tree();
    addChild(baum1);
    baum1->transform.position = glm::vec3(0.0f, 6.3f, 0.0f);

    auto* baum2 = new Tree();
    addChild(baum2);
    baum2->transform.position = glm::vec3(5.0f, 5.2f, -3.0f);

    auto* baum3 = new Tree();
    addChild(baum3);
    baum3->transform.position = glm::vec3(-6.0f, 4.9f, -4.0f);

    auto* baum4 = new Tree();
    addChild(baum4);
    baum4->transform.position = glm::vec3(5.0f, 4.78f, 3.0f);

    auto* baum5 = new Tree();
    addChild(baum5);
    baum5->transform.position = glm::vec3(-3.0f, 4.87f, 4.0f);

    auto* baum6 = new Tree();
    addChild(baum6);
    baum6->transform.position = glm::vec3(-6.0f, 5.37f, 0.0f);

    auto* baum7 = new Tree();
    addChild(baum7);
    baum7->transform.position = glm::vec3(2.0f, 4.95f, -5.0f);

    auto* baum8 = new Tree();
    addChild(baum8);
    baum8->transform.position = glm::vec3(6.0f, 5.06f, -3.0f);

    auto* baum9 = new Tree();
    addChild(baum9);
    baum9->transform.position = glm::vec3(-3.0f, 4.75f, -6.0f);

    isCollidable = true;
    buildGrid();
}

void Island::buildGrid() {
    grid.clear();
    for (int i = 0; i < static_cast<int>(vertices.size()); i++) {
        int gx = static_cast<int>(glm::floor(vertices[i].x / gridCellSize));
        int gz = static_cast<int>(glm::floor(vertices[i].z / gridCellSize));
        grid[gridKey(gx, gz)].push_back(i);
    }
}

float Island::getHeight(const float x, const float z) const {
    glm::vec3 localQuery = (glm::vec3(x, 0.0f, z) - transform.position) / transform.scale;

    int gx = static_cast<int>(glm::floor(localQuery.x / gridCellSize));
    int gz = static_cast<int>(glm::floor(localQuery.z / gridCellSize));

    float closestDistSq = std::numeric_limits<float>::max();
    float groundHeight = -1.0f;

    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
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

void Island::graben(glm::vec3 weltPos, float radius, float tiefe) {
    auto [position, rotation, scale] = getGlobalTransform();
    glm::quat invRot = glm::inverse(rotation);

    glm::vec3 lokalPos = invRot * (weltPos - position) / scale;

    bool geaendert = false;

    float lokalRadius = radius / scale.x;

    for (auto& vert : vertices) {
        float dx = vert.x - lokalPos.x;
        float dz = vert.z - lokalPos.z;
        float distSq = dx * dx + dz * dz;
        float rSq = lokalRadius * lokalRadius;

        if (distSq < rSq) {
            float faktor = 1.0f - (distSq / rSq);
            vert.y -= (tiefe / scale.y) * faktor;
            geaendert = true;
        }
    }

    if (geaendert) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
            static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
            vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
