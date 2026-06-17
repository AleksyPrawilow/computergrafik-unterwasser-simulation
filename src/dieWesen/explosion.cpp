#include "explosion.h"
#include "../werkzeuge/textur.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"

extern Kamera kamera;

Explosion::Explosion(const glm::vec3& position, float groesse)
    : startPosition(position), maxGroesse(groesse) {
    name = "explosion";
}

void Explosion::init() {
    kamera.addShake(0.3f, 0.3f);
    transform.position = startPosition;
    transform.scale = glm::vec3(0.1f);

    loadModel("assets/models/sphere.obj");
    material.albedo = Kern::LoadTexture("assets/textures/laser_rot.png");
    material.emission = Kern::LoadTexture("assets/textures/laser_rot.png");

    material.shader = ShaderManager::getInstance().loadShader(
        "explosion", "assets/shaders/explosion.vert", "assets/shaders/explosion.frag"
    );

    material.isTransparent = true;

    cachedGlobalModelMatrix = transform.getModelMatrix();
    cachedGlobalTransform = transform;
}

void Explosion::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    alter += deltaTime;
    float t = alter / lebensdauer;

    if (t >= 1.0f) {
        queueDestroy();
        return;
    }

    float skala = maxGroesse * glm::sin(t * 1.57079f);
    transform.scale = glm::vec3(skala);
}

void Explosion::prepareUniforms() const {
    float t = alter / lebensdauer;
    Kern::setUniform(material.shader, "u_progress", t);
}