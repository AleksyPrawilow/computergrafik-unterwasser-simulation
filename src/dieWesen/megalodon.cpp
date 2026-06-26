//
// Created by Alexey Pravilov on 25/06/2026.
//

#include "megalodon.h"

#include "leviathan.h"
#include "ui/cinematicBars.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"

void Megalodon::init() {
    loadModel("assets/models/shark.obj");
    material.shader = ShaderManager::getInstance().loadShader(
        "megalodon",
        "assets/shaders/fish.vert",
        "assets/shaders/default.frag"
        );
    material.albedo = Kern::LoadTexture("assets/textures/shark_albedo.jpg");
    material.normal = Kern::LoadTexture("assets/textures/shark_normal.png");

    auto * teeth = new Wesen();
    teeth->material.shader = material.shader;
    teeth->material.albedo = Kern::LoadTexture("assets/textures/shark_teeth_albedo.png");
    teeth->loadModel("assets/models/shark_teeth.obj");
    addChild(teeth);

    auto * eyes = new Wesen();
    eyes->material.shader = material.shader;
    eyes->material.albedo = Kern::LoadTexture("assets/textures/shark_eyes_albedo.png");
    eyes->loadModel("assets/models/shark_eyes.obj");
    addChild(eyes);

    addToGroup("F");

    transform.scale = glm::vec3(14.0f);
}

void Megalodon::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    float dist = glm::distance(cameraTransform.position, getGlobalTransform().position);
    if (dist < 100.0f && !triggeredCutscene) {
        triggeredCutscene = true;
        cutscene();
    }
    if (!isGrabbed) {
        transform.position = followTarget->getGlobalTransform().position;
        transform.rotation = glm::slerp(transform.rotation, followTarget->getGlobalTransform().rotation, 0.1f);
    }
}

void Megalodon::prepareUniforms() const {
    Kern::setUniform(material.shader, "time", static_cast<float>(glfwGetTime()));
}

void Megalodon::getEaten() {
    auto * leviathan = dynamic_cast<Leviathan *>(getNodesInGroup("Leviathan")[0]);
    leviathan->moveSpeed = 9.0f;
    leviathan->cutscene = false;
    auto * uboot = dynamic_cast<Uboot * >(getNodesInGroup("player")[0]);
    uboot->cameraFollowTarget = leviathan;

    queueDestroy();
    parent->createTween()
        ->tweenInterval(1.0f)
        ->tweenCallback([this, uboot]() {
            auto * cinematicBars = dynamic_cast<CinematicBars *>(getNodesInGroup("CinematicBars")[0]);
            cinematicBars->setEnabled(false);
            uboot->cameraFollowTarget = nullptr;
        });
}

void Megalodon::cutscene() {
    MusicManager::getInstance().playMusic("assets/audio/shark.mp3");
    auto * cinematicBars = dynamic_cast<CinematicBars *>(getNodesInGroup("CinematicBars")[0]);
    cinematicBars->setEnabled(true);
    auto * uboot = dynamic_cast<Uboot * >(getNodesInGroup("player")[0]);
    uboot->cameraFollowTarget = this;
    auto * leviathan = new Leviathan();
    leviathan->transform.position = glm::vec3(-144.0f, -264.0f, -944.0f);
    leviathan->moveSpeed = 14.0f;
    leviathan->cutscene = true;
    parent->addChild(leviathan);
    leviathan->transform.scale = glm::vec4(4.0f);
}
