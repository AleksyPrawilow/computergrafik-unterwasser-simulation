//
// Created by Alexey Pravilov on 19/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PROP_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PROP_H
#include "werkzeuge/wesen.h"

class Prop : public Wesen {
public:
    Prop(glm::vec3 position, glm::quat orientation, glm::vec3 scale);
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PROP_H
