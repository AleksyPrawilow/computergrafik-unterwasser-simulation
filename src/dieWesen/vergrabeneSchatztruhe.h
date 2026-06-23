#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_VERGRABENESCHATZTRUHE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_VERGRABENESCHATZTRUHE_H

#include "werkzeuge/wesen.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/ui/worldspaceUI.h"

class VergrabeneSchatztruhe : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    void graben();
    void oeffnen();
    void setMarkierung(Wesen* m) { markierung = m; }

private:
    int grabFortschritt = 0;
    static constexpr int GRABEN_BENOETIGT = 5;
    bool freigelegt = false;
    bool geoeffnet = false;

    Wesen* markierung = nullptr;
    UILabel* interactLabel = nullptr;
    Wesen* batyskaf = nullptr;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_VERGRABENESCHATZTRUHE_H
