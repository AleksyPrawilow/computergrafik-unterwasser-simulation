//
// Created by Alexey Pravilov on 26/06/2026.
//

#include "logo.h"

#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void Logo::init() {
    loadModel("assets/models/starwars.obj");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.opacity = Kern::LoadTexture("assets/textures/starwarslogo/starwarslogo.1.png");
    material.albedo = Kern::LoadTexture("assets/textures/starwarslogo/starwarslogo.1.png");
    material.emission = Kern::LoadTexture("assets/textures/starwarslogo/starwarslogo.1.png");
    material.bloomStrength = 0.8f;
    material.doubleSided = true;

    transform.position = kamera.transform.position + kamera.transform.forward() * 4.0f;
    transform.rotation = kamera.transform.rotation * glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
    transform.scale = glm::vec3(2.0f);

    createTween()
        ->tweenProperty(&transform.scale, glm::vec3(0.0f), 5.0f, EaseType::LINEAR);
}


void Logo::prepareUniforms() const {
    Kern::setUniform(material.shader, "alphaCutoff", 0.2f);
}
