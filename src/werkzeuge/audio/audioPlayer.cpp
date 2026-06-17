//
// Created by Alexey Pravilov on 14/06/2026.
//

#include "audioPlayer.h"
#include "audioManager.h"

AudioPlayer::AudioPlayer(const std::string& filepath, bool loop, float volume, bool isInterior)
    : streamFilepath(filepath), shouldLoop(loop), initialVolume(volume), isInteriorSound(isInterior) {}

AudioPlayer::~AudioPlayer() {
    if (hasSound) {
        ma_sound_uninit(&sound);
    }
}

void AudioPlayer::init() {
    ma_engine* engine = isInteriorSound
                        ? AudioManager::getInstance().getIntEngine()
                        : AudioManager::getInstance().getEnvEngine();
    if (engine == nullptr) return;

    ma_uint32 flags = shouldLoop ? MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_STREAM : MA_SOUND_FLAG_DECODE;

    ma_result result = ma_sound_init_from_file(engine, streamFilepath.c_str(), flags, NULL, NULL, &sound);
    if (result == MA_SUCCESS) {
        hasSound = true;
        ma_sound_set_volume(&sound, initialVolume);
        ma_sound_set_looping(&sound, shouldLoop ? MA_TRUE : MA_FALSE);

        glm::vec3 startPos = getGlobalTransform().position;
        ma_sound_set_position(&sound, startPos.x, startPos.y, startPos.z);
    } else {
        std::cerr << "Failed to load 3D sound stream: " << streamFilepath << std::endl;
    }
}

void AudioPlayer::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (hasSound) {
        const glm::vec3 worldPos = getGlobalTransform().position;
        ma_sound_set_position(&sound, worldPos.x, worldPos.y, worldPos.z);
    }
}

void AudioPlayer::play() {
    if (hasSound) {
        ma_sound_seek_to_pcm_frame(&sound, 0);
        ma_sound_start(&sound);
    }
}

void AudioPlayer::stop() {
    if (hasSound) {
        ma_sound_stop(&sound);
    }
}

bool AudioPlayer::isPlaying() const {
    if (!hasSound) return false;
    return ma_sound_is_playing(&sound) == MA_TRUE;
}