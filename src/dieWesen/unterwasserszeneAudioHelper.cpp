//
// Created by Alexey Pravilov on 21/06/2026.
//

#include "unterwasserszeneAudioHelper.h"

#include "werkzeuge/kamera.h"

extern Kamera kamera;

void UnterwasserszeneAudioHelper::init() {
    musicManager = &MusicManager::getInstance();
    setGameState(OVER_WATER);
    addToGroup("Music");
}

void UnterwasserszeneAudioHelper::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (isQueuedDestroyed || deaktiviert) return;
    switch (gameState) {
        case OVER_WATER:
            manageIsland(cameraTransform);
            break;
        case UNDER_WATER:
            manageUnderwater(cameraTransform);
            break;
        case ABYSS:
            manageAbyss(cameraTransform);
            break;
        case DEEP_ABYSS:
            manageDeepAbyss(cameraTransform);
            break;
        default:
            break;
    }
}

void UnterwasserszeneAudioHelper::setGameState(const GameState state) {
    gameState = state;
    if (isChasing) return;
    switch (gameState) {
        case OVER_WATER:
            musicManager->playMusic("assets/audio/island_music.mp3");
            break;
        case UNDER_WATER:
            musicManager->playMusic("assets/audio/underwater.mp3");
            break;
        case ABYSS:
            musicManager->playMusic("assets/audio/abyss.mp3");
            break;
        case DEEP_ABYSS:
            musicManager->playMusic("assets/audio/deep_abyss.mp3");
            break;
        default:
            break;
    }
}

void UnterwasserszeneAudioHelper::initiateChase() {
    if (isChasing) return;
    isChasing = true;
    musicManager->playMusic("assets/audio/chase.mp3");
}

void UnterwasserszeneAudioHelper::stopChasing() {
    if (!isChasing) return;
    isChasing = false;
    setGameState(gameState);
}

void UnterwasserszeneAudioHelper::manageIsland(const Transform& cameraTransform) {
    if (cameraTransform.position.y < -1.0f) {
        setGameState(UNDER_WATER);
    }
}
void UnterwasserszeneAudioHelper::manageUnderwater(const Transform& cameraTransform) {
    if (cameraTransform.position.y > 0.0f) {
        setGameState(OVER_WATER);
    }
    if (cameraTransform.position.y < -100.0f) {
        setGameState(ABYSS);
    }
}
void UnterwasserszeneAudioHelper::manageAbyss(const Transform& cameraTransform) {
    if (cameraTransform.position.y > -95.0f) {
        setGameState(UNDER_WATER);
    }

    if (cameraTransform.position.y < -200.0f) {
        setGameState(DEEP_ABYSS);
    }
}

void UnterwasserszeneAudioHelper::manageDeepAbyss(const Transform& cameraTransform) {
    if (cameraTransform.position.y > -198.0f) {
        setGameState(ABYSS);
    }
}
