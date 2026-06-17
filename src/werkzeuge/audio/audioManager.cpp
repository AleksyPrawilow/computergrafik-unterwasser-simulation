//
// Created by Alexey Pravilov on 14/06/2026.
//

#include "audioManager.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

void AudioManager::init() {
    ma_result result;

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format    = ma_format_f32;
    deviceConfig.playback.channels  = 2;
    deviceConfig.sampleRate         = 44100;
    deviceConfig.dataCallback       = audio_data_callback;
    deviceConfig.pUserData          = nullptr;

    result = ma_device_init(NULL, &deviceConfig, &device);
    result = ma_device_start(&device);

    ma_engine_config envConfig = ma_engine_config_init();
    envConfig.noDevice = MA_TRUE;
    envConfig.channels = 2;
    envConfig.sampleRate = 44100;
    result = ma_engine_init(&envConfig, &envEngine);

    ma_engine_config intConfig = ma_engine_config_init();
    intConfig.pDevice = &device;
    result = ma_engine_init(&intConfig, &intEngine);

    ma_lpf_config lpfConfig = ma_lpf_config_init(ma_format_f32, 2, 44100, 400.0f, 2);
    result = ma_lpf_init(&lpfConfig, NULL, &lpf);

    initialized = true;
}

void AudioManager::play2D(const std::string& filepath, bool loop, bool isInterior) {
    if (!initialized) return;
    ma_engine* targetEngine = isInterior ? &intEngine : &envEngine;

    ma_sound* sound = new ma_sound();
    ma_uint32 flags = loop ? MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_STREAM : MA_SOUND_FLAG_DECODE;

    ma_result result = ma_sound_init_from_file(targetEngine, filepath.c_str(), flags, NULL, NULL, sound);
    if (result == MA_SUCCESS) {
        ma_sound_set_spatialization_enabled(sound, MA_FALSE);
        ma_sound_set_looping(sound, loop ? MA_TRUE : MA_FALSE);
        ma_sound_start(sound);
        active2DSounds.push_back(sound);
    } else {
        std::cerr << "AUDIO ERROR: Failed to load 2D sound file: " << filepath
            << " | Error code: " << result << std::endl;
        delete sound;
    }
}

void AudioManager::play3D(const std::string& filepath, const glm::vec3& position, float volume, bool isInterior) {
    if (!initialized) return;
    ma_engine* targetEngine = isInterior ? &intEngine : &envEngine;

    ma_sound* sound = new ma_sound();

    ma_result result = ma_sound_init_from_file(targetEngine, filepath.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, sound);
    if (result == MA_SUCCESS) {
        ma_sound_set_min_distance(sound, 5.0f);
        ma_sound_set_max_distance(sound, 60.0f);
        ma_sound_set_rolloff(sound, 1.0f);

        ma_sound_set_position(sound, position.x, position.y, position.z);
        ma_sound_set_volume(sound, volume);
        ma_sound_start(sound);
        active3DSounds.push_back(sound);
    } else {
        std::cerr << "AUDIO ERROR: Failed to load 3D sound file: " << filepath
            << " | Error code: " << result << std::endl;
        delete sound;
    }
}

void AudioManager::updateListener(const glm::vec3& camPos, const glm::vec3& camForward, const glm::vec3& camUp) {
    if (!initialized) return;

    ma_engine_listener_set_position(&envEngine, 0, camPos.x, camPos.y, camPos.z);
    ma_engine_listener_set_direction(&envEngine, 0, camForward.x, camForward.y, camForward.z);
    ma_engine_listener_set_world_up(&envEngine, 0, camUp.x, camUp.y, camUp.z);

    ma_engine_listener_set_position(&intEngine, 0, camPos.x, camPos.y, camPos.z);
    ma_engine_listener_set_direction(&intEngine, 0, camForward.x, camForward.y, camForward.z);
    ma_engine_listener_set_world_up(&intEngine, 0, camUp.x, camUp.y, camUp.z);

    for (auto it = active3DSounds.begin(); it != active3DSounds.end();) {
        if (ma_sound_is_playing(*it) == MA_FALSE && ma_sound_at_end(*it) == MA_TRUE) {
            ma_sound_uninit(*it);
            delete *it;
            it = active3DSounds.erase(it);
        } else {
            ++it;
        }
    }
}

void AudioManager::allesStoppen() {
    for (auto* s : active2DSounds) {
        ma_sound_stop(s);
        ma_sound_uninit(s);
        delete s;
    }
    active2DSounds.clear();

    for (auto* s : active3DSounds) {
        ma_sound_stop(s);
        ma_sound_uninit(s);
        delete s;
    }
    active3DSounds.clear();
}

void AudioManager::shutdown() {
    if (!initialized) return;

    for (auto* s : active2DSounds) {
        ma_sound_uninit(s);
        delete s;
    }
    active2DSounds.clear();

    for (auto* s : active3DSounds) {
        ma_sound_uninit(s);
        delete s;
    }
    active3DSounds.clear();

    ma_device_uninit(&device);
    ma_engine_uninit(&envEngine);
    ma_engine_uninit(&intEngine);
    ma_lpf_uninit(&lpf, NULL);
    initialized = false;
}

void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_engine* pEnvEngine = AudioManager::getInstance().getEnvEngine();
    ma_engine* pIntEngine = AudioManager::getInstance().getIntEngine();

    if (pEnvEngine == nullptr || pIntEngine == nullptr) return;

    ma_engine_read_pcm_frames(pEnvEngine, pOutput, frameCount, NULL);

    if (AudioManager::getInstance().isMuffled()) {
        ma_lpf_process_pcm_frames(&AudioManager::getInstance().getLPF(), pOutput, pOutput, frameCount);
    }

    std::vector<float>& mixBuf = AudioManager::getInstance().getMixBuffer();
    ma_uint32 totalSamples = frameCount * 2; // Stereo

    if (mixBuf.size() < totalSamples) {
        mixBuf.resize(totalSamples, 0.0f);
    }

    ma_engine_read_pcm_frames(pIntEngine, mixBuf.data(), frameCount, NULL);

    auto* out = (float*)pOutput;
    for (ma_uint32 i = 0; i < totalSamples; ++i) {
        out[i] += mixBuf[i];
    }
}
