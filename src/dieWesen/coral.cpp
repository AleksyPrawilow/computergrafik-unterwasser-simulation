//
// Created by Alexey Pravilov on 25/06/2026.
//

#include "coral.h"
#include "proceduralCoral.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

Coral::Coral(glm::vec3 position, glm::quat orientation, glm::vec3 scale, int seed)
    : Prop(position, orientation, scale), spawnSeed(seed) {}

void Coral::init() {
    name = "foliage"; // Grouped under foliage

    // 1. Generate the completely unique procedural mesh and populate the local AABB
    mesh = Kern::ProceduralCoral::generate(spawnSeed, localAABB);
    hasMesh = true;

    // 2. Setup high-emissive underwater materials
    material.shader = ShaderManager::getInstance().getShader("default");

    // Use an unlit white template and tint it coral-orange/pink inside the PBR pipeline
    material.albedo = Kern::LoadTexture("assets/textures/default_opacity.png");

    // Make the coral glow softly in deep water
    material.emission = Kern::LoadTexture("assets/textures/default_opacity.png");
    material.normal = Kern::LoadTexture("assets/textures/default_normal.png");

    // Adjust material properties
    material.roughness = Kern::LoadTexture("assets/textures/default_normal.png");
}