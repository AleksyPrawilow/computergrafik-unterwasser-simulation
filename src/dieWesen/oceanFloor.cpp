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
    loadModel("assets/models/oceanbed.obj");
    transform.scale = glm::vec3(1.0f);
}
