#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUFHEBBAR_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUFHEBBAR_H

#include "werkzeuge/wesen.h"
#include "werkzeuge/gegenstandDaten.h"

class Aufhebbar : public Wesen {
public:
    Aufhebbar(GegenstandID gegenstandId, int anzahl = 1);

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    GegenstandID gegenstandId;
    int anzahl;
    float bobZeit = 0.0f;
    float startHoehe = 0.0f;
    bool eingesammelt = false;

    void einsammeln();
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AUFHEBBAR_H
