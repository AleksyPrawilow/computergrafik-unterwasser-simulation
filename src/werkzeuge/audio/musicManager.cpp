//
// Created by Alexey Pravilov on 21/06/2026.
//

#include "musicManager.h"
#include "werkzeuge/visual/tween.h"

void MusicManager::init() {
    name = "MusicManager";
}

void MusicManager::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (playerA != nullptr && playerA->isPlaying()) {
        playerA->setVolume(volumeA * masterVolume);
    }
    if (playerB != nullptr && playerB->isPlaying()) {
        playerB->setVolume(volumeB * masterVolume);
    }
}

void MusicManager::playMusic(const std::string& filepath, float fadeDuration, bool loop) {
    if (isChannelAActive) {
        isChannelAActive = false;

        if (playerB != nullptr) {
            playerB->queueDestroy();
        }

        playerB = new AudioPlayer(filepath, loop, 0.0f, false, false);
        addChild(playerB);
        playerB->play();

        createTween()
            ->tweenProperty(&volumeA, 0.0f, fadeDuration, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&volumeB, 1.0f, fadeDuration, EaseType::EASE_IN_SINE)
            ->tweenCallback([this]() {
                if (playerA != nullptr) {
                    playerA->stop();
                }
            });
    }
    else {
        isChannelAActive = true;

        if (playerA != nullptr) {
            playerA->queueDestroy();
        }

        playerA = new AudioPlayer(filepath, loop, 0.0f, false, false);
        addChild(playerA);
        playerA->play();

        createTween()
            ->tweenProperty(&volumeB, 0.0f, fadeDuration, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&volumeA, 1.0f, fadeDuration, EaseType::EASE_IN_SINE)
            ->tweenCallback([this]() {
                if (playerB != nullptr) {
                    playerB->stop();
                }
            });
    }
}

void MusicManager::stopAll() {
    if (playerA != nullptr) {
        playerA->stop();
        playerA->queueDestroy();
        playerA = nullptr;
    }
    if (playerB != nullptr) {
        playerB->stop();
        playerB->queueDestroy();
        playerB = nullptr;
    }
    volumeA = 0.0f;
    volumeB = 0.0f;
    isChannelAActive = true;
}

void MusicManager::setMasterVolume(float volume) {
    masterVolume = glm::clamp(volume, 0.0f, 1.0f);
}