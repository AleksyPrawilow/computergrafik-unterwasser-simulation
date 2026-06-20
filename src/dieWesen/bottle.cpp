//
// Created by Alexey Pravilov on 20/06/2026.
//

#include "bottle.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Bottle::init() {
    material.albedo = Kern::LoadTexture("assets/textures/bottle_albedo.png");
    material.normal = Kern::LoadTexture("assets/textures/bottle_normal.png");
    material.roughness = Kern::LoadTexture("assets/textures/bottle_roughness.png");
    material.metallic = Kern::LoadTexture("assets/textures/bottle_roughness.png");
    material.opacity = Kern::LoadTexture("assets/textures/bottle_opacity.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.isTransparent = true;
    loadModel("assets/models/bottle.obj");

    cork = new Wesen();
    cork->material.albedo = Kern::LoadTexture("assets/textures/bottle_albedo.png");
    cork->material.normal = Kern::LoadTexture("assets/textures/bottle_normal.png");
    cork->material.roughness = Kern::LoadTexture("assets/textures/bottle_roughness.png");
    cork->material.metallic = Kern::LoadTexture("assets/textures/bottle_roughness.png");
    cork->material.shader = ShaderManager::getInstance().getShader("default");
    cork->loadModel("assets/models/bottle_cork.obj");
    addChild(cork);
}
