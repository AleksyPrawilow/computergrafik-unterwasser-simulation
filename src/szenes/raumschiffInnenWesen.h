//
// Created by Kajetan on 26/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAUMSCHIFFINNENWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAUMSCHIFFINNENWESEN_H
#include "werkzeuge/wesen.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/ui/wesenUI.h"

class Player;
class FadeOverlay;
class CinematicBars;

class RaumschiffInnenWesen : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    Player* spielerRef = nullptr;
    UILabel* lebenLabel = nullptr;
    UIElement* schadenVignette = nullptr;
    FadeOverlay* fadeOverlay = nullptr;
    CinematicBars* cinematicBars = nullptr;

    static constexpr int KOMPASS_MAX = 10;
    static constexpr float KOMPASS_BREITE = 500.0f;
    UIElement* kompassHG = nullptr;
    UIElement* kompassPunkte[KOMPASS_MAX] = {};

    bool cutsceneGestartet = false;
    bool cutsceneAktiv = false;
    glm::vec3 cameraZielPos = glm::vec3(0.0f, 4.0f, -30.0f);
    glm::vec3 cameraBlickZiel = glm::vec3(0.0f, 4.0f, -100.0f);
    Wesen* todessternKugel = nullptr;
    Wesen* leviathanModell = nullptr;
    Wesen* riesenLaser = nullptr;

    void kompassAktualisieren();
    void starteCutscene();
    void spawnRiesenLaser();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAUMSCHIFFINNENWESEN_H
