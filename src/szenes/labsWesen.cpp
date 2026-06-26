//
// Created by Alexey Pravilov on 25/06/2026.
//

#include "labsWesen.h"
#include "labsProps.h"
#include "dieWesen/player.h"
#include "dieWesen/prop.h"
#include "dieWesen/uboot.h"
#include "dieWesen/wasser.h"
#include "werkzeuge/himmelsboxWesen.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/worldEnvironment.h"

void LabsWesen::init() {
    addChild(new HimmelsboxWesen({
    "assets/textures/skybox_weltraum/px.png",
    "assets/textures/skybox_weltraum/nx.png",
    "assets/textures/skybox_weltraum/py.png",
    "assets/textures/skybox_weltraum/ny.png",
    "assets/textures/skybox_weltraum/pz.png",
    "assets/textures/skybox_weltraum/nz.png"
}));

    auto * umwelt = new WorldEnvironment();
    umwelt->init();
    umwelt->params.sunEnergy = 3.0f;
    umwelt->params.sunDirection = glm::normalize(glm::vec3(0.3f, 0.8f, 0.5f));
    umwelt->params.fogEnabled = false;
    umwelt->params.heightFogEnabled = false;
    umwelt->params.causticsEnabled = false;
    umwelt->params.depthDimmingEnabled = false;
    umwelt->params.bloomThreshold = 0.0f;
    umwelt->params.bloomIntensity = 1.5f;
    addChild(umwelt);

    addChild(new Island());
    auto * player = new Player();
    player->affectedByWater = false;
    addChild(player);

    addChild(new Wasser());
    auto * uboot = new Uboot();
    addChild(uboot);

    auto sceneData = getLabsProps();
    for (const auto& [className, transforms] : sceneData) {
        for (const auto& t : transforms) {

            Wesen * entity = nullptr;
            if (className == "cave") {
                entity = new Prop(t.position, t.rotation, t.scale);
                entity->loadModel("assets/models/cave.obj");
                entity->material.shader = ShaderManager::getInstance().getShader("default");
                entity->material.albedo = Kern::LoadTexture("assets/textures/RockTexture001_ao.png");
            } else if (className == "playerpos") {
                player->transform.position = t.position;
                player->targetY = t.position.y;
            } else if (className == "ubootpos") {
                uboot->transform.position = t.position;
            } else if (className == "bridge") {
                entity = new Prop(t.position, t.rotation, t.scale);
                entity->loadModel("assets/models/bridge.obj");
                entity->material.shader = ShaderManager::getInstance().getShader("default");
                entity->material.albedo = Kern::LoadTexture("assets/textures/bridge_albedo.png");
                entity->material.normal = Kern::LoadTexture("assets/textures/bridge_normal.png");
                entity->material.roughness = Kern::LoadTexture("assets/textures/bridge_roughness.png");
                entity->material.metallic = Kern::LoadTexture("assets/textures/bridge_roughness.png");
            } else if (className == "bridge_open") {
                entity = new Prop(t.position, t.rotation, t.scale);
                entity->loadModel("assets/models/bridge_opening.obj");
                entity->material.shader = ShaderManager::getInstance().getShader("default");
                entity->material.albedo = Kern::LoadTexture("assets/textures/bridge_albedo.png");
                entity->material.normal = Kern::LoadTexture("assets/textures/bridge_normal.png");
                entity->material.roughness = Kern::LoadTexture("assets/textures/bridge_roughness.png");
                entity->material.metallic = Kern::LoadTexture("assets/textures/bridge_roughness.png");
            } else if (className == "spaceship") {
                entity = new Prop(t.position, t.rotation, t.scale);
                entity->loadModel("assets/models/spaceship2.obj");
                entity->material.shader = ShaderManager::getInstance().getShader("default");
                entity->material.albedo = Kern::LoadTexture("assets/textures/raumschiff_albedo.png");
                entity->material.roughness = Kern::LoadTexture("assets/textures/raumschiff_roughness.png");
                entity->material.metallic = Kern::LoadTexture("assets/textures/raumschiff_metallic.png");
                entity->material.normal = Kern::LoadTexture("assets/textures/raumschiff_normal.png");

            } else if (className == "platform") {
                entity = new Prop(t.position, t.rotation, t.scale);
                entity->loadModel("assets/models/platform.obj");
                entity->material.shader = ShaderManager::getInstance().getShader("default");
                entity->material.albedo = Kern::LoadTexture("assets/textures/platform_albedo.png");
                entity->material.normal = Kern::LoadTexture("assets/textures/platform_normal.png");
                entity->material.metallic = Kern::LoadTexture("assets/textures/platform_metallic.png");
                entity->material.roughness = Kern::LoadTexture("assets/textures/platform_roughness.png");
            }
            if (entity != nullptr) {
                addChild(entity);
                entity->transform.position = t.position;
                entity->transform.rotation = t.rotation;
                entity->transform.scale = t.scale;
                std::cout << entity->transform.position.x << " " << entity->transform.position.y <<  " " << entity->transform.position.z << std::endl;
            }
        }
    }
}
