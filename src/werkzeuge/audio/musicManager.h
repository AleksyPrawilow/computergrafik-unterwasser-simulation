//
// Created by Alexey Pravilov on 21/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MUSICMANAGER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MUSICMANAGER_H

#include "werkzeuge/wesen.h"
#include "audioPlayer.h"

class MusicManager : public Wesen {
public:
    static MusicManager& getInstance() {
        static MusicManager instance;
        return instance;
    }

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    [[nodiscard]] bool hasCustomRender() const override { return true; };
    void playMusic(const std::string& filepath, float fadeDuration = 2.0f, bool loop = true);
    void stopAll();
    void setMasterVolume(float volume);
    [[nodiscard]] float getMasterVolume() const { return masterVolume; }

private:
    MusicManager() = default;
    ~MusicManager() override = default;

    AudioPlayer * playerA = nullptr;
    AudioPlayer * playerB = nullptr;

    float volumeA = 0.0f;
    float volumeB = 0.0f;
    float masterVolume = 0.5f;

    bool isChannelAActive = true;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MUSICMANAGER_H
