//
// Created by Alexey Pravilov on 22/06/2026.
//

#include "thunderstorm.h"
#include "werkzeuge/visual/worldEnvironment.h"
#include "werkzeuge/visual/tween.h"
#include "werkzeuge/random.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "gtc/type_ptr.hpp"
#include <iostream>

#include "rainEmitter.h"

extern Kamera kamera;

// Specialized internal Wesen class to safely bind custom uniforms during standard draw passes
class LightningBolt : public Wesen {
public:
    const float* progressRef = nullptr;

    void prepareUniforms() const override {
        if (progressRef != nullptr) {
            // Write the current progress value directly to the active shader
            Kern::setUniform(material.shader, "u_progress", *progressRef);
        }
    }
};

void Thunderstorm::init() {
    name = "ThunderstormController";
    addToGroup("thunderstorm");

    thunderSounds[0] = new AudioPlayer("assets/audio/thunder1.mp3", false, 1.0f, false, false);
    addChild(thunderSounds[0]);

    thunderSounds[1] = new AudioPlayer("assets/audio/thunder2.mp3", false, 1.0f, false, false);
    addChild(thunderSounds[1]);


    thunderSounds[2] = new AudioPlayer("assets/audio/thunder3.mp3", false, 1.0f, false, false);
    addChild(thunderSounds[2]);

    strikeTimer = new Timer();
    addChild(strikeTimer);

    rainEmitter = new RainEmitter(6000);
    rainEmitter->active = false;
    addChild(rainEmitter);

    // 4. Configure the procedural lightning bolt billboard
    lightningBolt = new LightningBolt();
    lightningBolt->name = "lightning"; // Bypasses frustum culling so it is never discarded
    lightningBolt->progressRef = &lightningProgress;

    // Load a flat plane model
    lightningBolt->loadModel("assets/models/cube.obj");

    lightningBolt->material.shader = ShaderManager::getInstance().loadShader(
        "lightning",
        "assets/shaders/default.vert",
        "assets/shaders/lightning.frag"
    );
    lightningBolt->material.bloomStrength = 1.0f;
    lightningBolt->material.isTransparent = true;
    lightningBolt->visible = false; // Hidden by default
    addChild(lightningBolt);
}

void Thunderstorm::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (rainEmitter == nullptr) return;

    // Pin the rain emitter above the camera and offset it slightly forward
    glm::vec3 cameraWorldPos = cameraTransform.position;
    glm::vec3 forwardOffset = cameraTransform.forward() * 4.0f;
    rainEmitter->transform.position = cameraWorldPos + glm::vec3(0.0f, 15.0f, 0.0f) + forwardOffset;
    rainEmitter->transform.scale = glm::vec3(30.0f, 1.0f, 30.0f);
}

void Thunderstorm::triggerLightning(glm::vec3 customPos, int soundId) {
    if (WorldEnvironment::activeEnv == nullptr || lightningBolt == nullptr) return;

    auto& params = WorldEnvironment::activeEnv->params;

    baseAmbientEnergy = params.ambientEnergy;
    baseSunEnergy = params.sunEnergy;
    baseFogColor = params.fogColor;

    glm::vec3 camPos = kamera.transform.position;
    glm::vec3 camForward = kamera.transform.forward();
    glm::vec3 camRight = kamera.transform.right();

    float sideOffset = Random::range(-90.0f, 90.0f);
    float quadHeight = 220.0f;
    glm::vec3 boltPos;

    if (glm::length2(customPos) == 0) {
        boltPos = camPos + (camForward * 260.0f) + (camRight * sideOffset) + glm::vec3(0.0f, quadHeight / 2.0f, 0.0f);
    } else {
        boltPos = customPos + glm::vec3(0.0f, quadHeight / 2.0f, 0.0f);
    }

    lightningBolt->transform.position = boltPos;
    lightningBolt->transform.lookAt(camPos, glm::vec3(0.0f, 1.0f, 0.0f));
    lightningBolt->transform.scale = glm::vec3(80.0f, 220.0f, 1.0f);

    lightningProgress = 0.0f;
    lightningBolt->visible = true;

    params.ambientEnergy = 1.3f;
    params.sunEnergy = 6.0f;
    params.fogColor = glm::vec3(0.85f, 0.90f, 1.0f);

    kamera.addShake(0.35f, 0.40f);

    createTween()
        ->tweenProperty(&lightningProgress, 1.0f, 0.15f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&params.ambientEnergy, 0.40f, 0.10f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&params.sunEnergy, 1.5f, 0.10f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&params.ambientEnergy, 0.85f, 0.08f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&params.sunEnergy, 4.0f, 0.08f, EaseType::EASE_OUT_SINE)
        ->tweenCallback([this]() {
            this->lightningBolt->visible = false;
        })
            ->tweenCallback([this, soundId]() {
                this->playThunderSound(soundId);
                float nextStrike = Random::range(3.0f, 6.0f);
                if (soundId == -1) {
                    strikeTimer->startTimer(nextStrike, [this]() { this->triggerLightning(); });
                }
        })
        ->tweenProperty(&params.ambientEnergy, baseAmbientEnergy, 0.65f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&params.sunEnergy, baseSunEnergy, 0.65f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&params.fogColor.r, baseFogColor.r, 0.65f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&params.fogColor.g, baseFogColor.g, 0.65f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&params.fogColor.b, baseFogColor.b, 0.65f, EaseType::EASE_OUT_SINE);
}

void Thunderstorm::beginThunderstorm() {
    rainEmitter->active = true;
    auto& params = WorldEnvironment::activeEnv->params;

    createTween()
        ->tweenProperty(&params.sunEnergy, 0.1f, 5.0f, EaseType::EASE_OUT_SINE);

    strikeTimer->startTimer(Random::range(5.0f, 12.0f), [this]() {
        this->triggerLightning();
    });
}

void Thunderstorm::playThunderSound(int soundId) {
    int idx = (soundId == -1) ? static_cast<int>(Random::range(0.0f, 3.0f)) : soundId;
    if (thunderSounds[idx] != nullptr) {
        thunderSounds[idx]->play();
    }
}
