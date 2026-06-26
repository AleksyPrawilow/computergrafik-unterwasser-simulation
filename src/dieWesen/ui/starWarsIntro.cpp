//
// Created by Alexey Pravilov on 26/06/2026.
//

#include "starWarsIntro.h"

#include "logo.h"
#include "dieWesen/raumschiff.h"
#include "szenes/weltraumszeneWesen.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/musicManager.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void StarWarsIntro::init() {
    loadModel("assets/models/starwars.obj");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.opacity = Kern::LoadTexture("assets/textures/starwarsintro/starwarsintro.1.png");
    material.albedo = Kern::LoadTexture("assets/textures/starwarsintro/starwarsintro.1.png");
    material.doubleSided = true;

    MusicManager::getInstance().playMusic("assets/audio/star_wars.mp3");

    transform.position = kamera.transform.position + kamera.transform.forward() * 4.0f;
    transform.rotation = kamera.transform.rotation * glm::quat(glm::radians(glm::vec3(-45.0f, 0.0f, 0.0f)));
    transform.position = transform.position - transform.up() * 4.0f;
    transform.scale = glm::vec3(1.0f);

    createTween()
        ->tweenProperty(&transform.position, kamera.transform.position + transform.up() * 5.0f, 60.0f, EaseType::LINEAR)
        ->tweenCallback([this]() {
            auto * player = dynamic_cast<Raumschiff*>(getNodesInGroup("spieler")[0]);
            auto * scene = dynamic_cast<WeltraumszeneWesen *>(parent);
            player->cutscene = false;
            scene->welleStarten(0);
        });

    parent->addChild(new Logo());
}

void StarWarsIntro::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    Wesen::onUpdate(window, deltaTime, cameraTransform);
}

void StarWarsIntro::prepareUniforms() const {
    Kern::setUniform(material.shader, "alphaCutoff", 0.2f);
}
