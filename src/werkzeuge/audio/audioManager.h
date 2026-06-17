//
// Created by Alexey Pravilov on 14/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUDIOMANAGER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUDIOMANAGER_H

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <glm.hpp>
#include "gtc/type_ptr.hpp"
#include "miniaudio.h"

inline void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

class AudioManager {
public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    void init();

    void play2D(const std::string& filepath, bool loop, bool isInterior = false);

    void play3D(const std::string& filepath, const glm::vec3& position, float volume = 1.0f, bool isInterior = false);

    ma_engine* getEnvEngine() { return initialized ? &envEngine : nullptr; }
    ma_engine* getIntEngine() { return initialized ? &intEngine : nullptr; }
    ma_lpf& getLPF() { return lpf; }
    std::vector<float>& getMixBuffer() { return mixBuffer; }

    void updateListener(const glm::vec3& camPos, const glm::vec3& camForward, const glm::vec3& camUp);
    void setMuffled(bool enable) { muffleEnabled = enable; }
    bool isMuffled() const { return muffleEnabled; }

    void allesStoppen();
    void shutdown();

private:
    AudioManager() = default;
    ~AudioManager() { shutdown(); }

    ma_engine envEngine;
    ma_engine intEngine;
    ma_device device;
    ma_lpf lpf;
    std::vector<float> mixBuffer;
    std::vector<ma_sound*> active2DSounds;
    std::vector<ma_sound*> active3DSounds;
    bool muffleEnabled = false;
    bool initialized = false;
};

void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUDIOMANAGER_H
