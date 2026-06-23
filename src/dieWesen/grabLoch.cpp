#include "grabLoch.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void GrabLoch::init() {
    loadModel("assets/models/cube.obj");
    material.albedo = Kern::LoadTexture("assets/textures/icon_erde.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.scale = glm::vec3(1.2f, 0.05f, 1.2f);
    name = "grabloch";
}
