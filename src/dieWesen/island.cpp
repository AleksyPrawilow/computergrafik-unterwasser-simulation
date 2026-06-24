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
