//
// Created by Alexey Pravilov on 04/06/2026.
//

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_EARTH_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_EARTH_H
#include "werkzeuge/wesen.h"


class Earth: public Wesen {
public:
    Wesen rock;
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_EARTH_H
