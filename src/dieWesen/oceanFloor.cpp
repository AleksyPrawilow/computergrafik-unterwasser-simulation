//
// Created by Alexey Pravilov on 08/06/2026.
//

#include "oceanFloor.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void OceanFloor::init() {
    material.albedo = Kern::LoadTexture("assets/textures/Ground_baseColor.png");
    material.normal = Kern::LoadTexture("assets/textures/Ground_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/Ground_metallicRoughness.png");
    material.roughness = Kern::LoadTexture("assets/textures/Ground_metallicRoughness.png");
    transform.position = glm::vec3(0.0f, -170.0f, 0.0f);
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    loadModel("assets/models/ocean_floor.obj");
    transform.scale = glm::vec3(15.0f);
}
