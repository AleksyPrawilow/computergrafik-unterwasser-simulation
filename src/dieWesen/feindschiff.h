#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FEINDSCHIFF_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_FEINDSCHIFF_H

#include "../werkzeuge/wesen.h"
#include "timer.h"
#include <vector>

enum class FeindTyp {
    PATROUILLE,
    JAEGER,
    ORBITER
};

class Feindschiff : public Wesen {
public:
    std::vector<glm::vec3> wegpunkte;
    float leben = 1.0f;
    float laserSchaden = 10.0f;
    glm::vec3 laserGroesse = glm::vec3(0.05f, 0.05f, 2.5f);
    float laserOffset = 1.5f;
    FeindTyp typ = FeindTyp::PATROUILLE;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void schadenNehmen(float schaden);

    float bewegungsGeschwindigkeit = 5.0f;
    float drehGeschwindigkeit = 2.0f;
    float schussIntervall = 2.0f;
    bool istTot = false;

private:
    int aktuellerWegpunkt = 0;
    float schussTimer = 0.0f;
    float orbitWinkel = 0.0f;
    float orbitRadius = 60.0f;
    float feuerTimer = 0.0f;

    void updatePatrouille(float deltaTime);
    void updateJaeger(float deltaTime);
    void updateOrbiter(float deltaTime);
    Wesen* findeSpieler() const;
    void laserAbfeuern();
    void laserAbfeuernAufZiel(const glm::vec3& zielPos);
};

#endif
