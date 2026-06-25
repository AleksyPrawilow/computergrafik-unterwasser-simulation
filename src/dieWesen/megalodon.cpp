//
// Created by Alexey Pravilov on 25/06/2026.
//

#include "megalodon.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

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

    transform.scale = glm::vec3(14.0f);
}

void Megalodon::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    float dist = glm::distance(cameraTransform.position, transform.position);
    if (dist < 50.0f && !triggeredCutscene) {
        triggeredCutscene = true;
    }
}

void Megalodon::prepareUniforms() const {
    Kern::setUniform(material.shader, "time", static_cast<float>(glfwGetTime()));
}

void Megalodon::cutscene() {

}
