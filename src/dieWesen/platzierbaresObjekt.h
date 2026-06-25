#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLATZIERBARESOBJEKT_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLATZIERBARESOBJEKT_H

#include "werkzeuge/wesen.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/visual/lightManager.h"

class PlatzierbaresObjekt : public Wesen {
public:
    PlatzierbaresObjekt(GegenstandID typ);
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    GegenstandID getTyp() const { return typ; }
    bool istWerkbank() const { return typ == GegenstandID::WERKBANK; }
    bool istOfen() const { return typ == GegenstandID::OFEN; }

private:
    GegenstandID typ;
    PointLight* torchLight = nullptr;
    Wesen* makePanel(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale);
    void buildHouse();
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PLATZIERBARESOBJEKT_H
