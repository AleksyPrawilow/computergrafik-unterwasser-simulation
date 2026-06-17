#include "laser.h"
#include "raumschiff.h"

#include "../werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"

void Laser::init() {
    loadModel("assets/models/cube.obj");
    transform.scale = glm::vec3(0.08f, 0.08f, 1.5f);
    material.albedo = Kern::LoadTexture("assets/textures/laser_rot.png");
    material.emission = Kern::LoadTexture("assets/textures/laser_rot.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    boundingRadius = 0.3f;
    addToGroup("feindlaser");
    name = "laser";
    visible = false;

    selbstzerstoerungsTimer = new Timer();
    addChild(selbstzerstoerungsTimer);
}

void Laser::abfeuern(const glm::vec3& position, const glm::quat& rotation) {
    transform.position = position;
    transform.rotation = rotation;
    istAbgefeuert = true;
    visible = true;
    updateGlobalTransforms();
    selbstzerstoerungsTimer->startTimer(lebensdauer, [this]() { queueDestroy(); });
}

void Laser::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!istAbgefeuert) return;

    transform.position += transform.forward() * geschwindigkeit * deltaTime;

    const auto& spieler = getNodesInGroup("spieler");
    for (auto* ziel : spieler) {
        float abstand = glm::distance(getGlobalTransform().position, ziel->getGlobalTransform().position);
        if (abstand < boundingRadius + ziel->boundingRadius) {
            if (auto* schiff = dynamic_cast<Raumschiff*>(ziel)) {
                schiff->schadenNehmen(schaden);
            }
            queueDestroy();
            return;
        }
    }
}
