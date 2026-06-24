//
// Created by Alexey Pravilov on 22/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_THUNDERSTORM_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_THUNDERSTORM_H

#include "particleEmitter.h"
#include "timer.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/audio/audioPlayer.h"

class LightningBolt;

class Thunderstorm : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void triggerLightning(glm::vec3 customPos = glm::vec3(0.0f), int soundId = -1);
    void beginThunderstorm();
private:
    void playThunderSound(int soundId = -1);

    Timer* strikeTimer = nullptr;
    AudioPlayer* thunderSounds[3] = { nullptr };
    ParticleEmitter* rainEmitter = nullptr;

    LightningBolt* lightningBolt = nullptr;
    float lightningProgress = 0.0f;

    float baseAmbientEnergy = 0.0f;
    float baseSunEnergy = 0.0f;
    glm::vec3 baseFogColor = glm::vec3(0.0f);
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_THUNDERSTORM_H
