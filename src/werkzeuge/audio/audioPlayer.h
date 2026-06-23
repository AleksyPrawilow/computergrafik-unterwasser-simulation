//
// Created by Alexey Pravilov on 14/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUDIOPLAYER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUDIOPLAYER_H
#include "werkzeuge/wesen.h"
#include "audioManager.h"

class AudioPlayer : public Wesen {
public:
    AudioPlayer(const std::string& filepath, bool loop = false, float volume = 1.0f, bool isInterior = false, bool isSpatial = true);
    ~AudioPlayer() override;

    void play();
    void stop();
    [[nodiscard]] bool isPlaying() const;
    void setVolume(float volume);
    [[nodiscard]] float getVolume() const { return initialVolume; }

    bool hasCustomRender() const override { return true; }
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override {}

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    std::string streamFilepath;
    bool shouldLoop = false;
    float initialVolume = 1.0f;
    bool isInteriorSound = false;;
    bool isSpatialSound = true;

    ma_sound sound;
    bool hasSound = false;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUDIOPLAYER_H
