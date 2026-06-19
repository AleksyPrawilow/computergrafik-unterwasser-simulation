//
// Created by Alexey Pravilov on 19/06/2026.
//

#include "rock.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Rock::init() {
    material.shader = ShaderManager::getInstance().getShader("default");
    material.albedo = Kern::LoadTexture("assets/textures/RockTexture001_ao.png");
    material.normal = Kern::LoadTexture("assets/textures/RockTexture001_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/RockTexture001_metallic.png");
    loadModel("assets/models/Rock001.obj");
}
