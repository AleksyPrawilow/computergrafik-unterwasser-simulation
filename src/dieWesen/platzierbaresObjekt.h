#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLATZIERBARESOBJEKT_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLATZIERBARESOBJEKT_H

#include "werkzeuge/wesen.h"
#include "werkzeuge/gegenstandDaten.h"

class PlatzierbaresObjekt : public Wesen {
public:
    PlatzierbaresObjekt(GegenstandID typ);
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    GegenstandID getTyp() const { return typ; }
    bool istWerkbank() const { return typ == GegenstandID::WERKBANK; }

private:
    GegenstandID typ;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLATZIERBARESOBJEKT_H
