#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WELTRAUMSZENEWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WELTRAUMSZENEWESEN_H

#include "dieWesen/timer.h"
#include "werkzeuge/wesen.h"

class Raumschiff;

class WeltraumszeneWesen : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    static void abschussZaehlen() { abschuesse++; }
    static int getAbschuesse() { return abschuesse; }

private:
    static int abschuesse;
    int aktuelleWelle = 0;
    bool welleAktiv = false;
    Timer* wellenTimer = nullptr;
    Timer* spawnDelay = nullptr;
    Raumschiff* raumschiff = nullptr;

    void welleStarten(int welle);
    void spawnWave(int numA, int numB, bool mitBoss);
    void alleFeindeEntfernen();
    void wellenPruefung();
};

#endif
