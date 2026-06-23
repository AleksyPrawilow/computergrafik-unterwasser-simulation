#include "platzierbaresObjekt.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

PlatzierbaresObjekt::PlatzierbaresObjekt(GegenstandID typ) : typ(typ) {}

void PlatzierbaresObjekt::init() {
    loadModel("assets/models/cube.obj");
    isCollidable = true;

    const auto& info = GegenstandDaten::getInstance().getInfo(typ);
    material.albedo = info.iconTextur;
    material.shader = ShaderManager::getInstance().getShader("default");

    if (typ == GegenstandID::WERKBANK) {
        name = "werkbank";
        transform.scale = glm::vec3(1.0f, 0.8f, 1.0f);
        addToGroup("werkbank");
    } else if (typ == GegenstandID::ZAUN) {
        name = "zaun";
        transform.scale = glm::vec3(1.5f, 1.2f, 0.2f);
    } else if (typ == GegenstandID::TRUHE) {
        name = "truhe";
        transform.scale = glm::vec3(0.8f, 0.6f, 0.6f);
    } else {
        name = "platziert";
        transform.scale = glm::vec3(0.5f);
    }
}

void PlatzierbaresObjekt::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
}
