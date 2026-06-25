//
// Created by Alexey Pravilov on 25/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CORAL_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CORAL_H

#include "prop.h"

class Coral : public Prop {
public:
    Coral(glm::vec3 position, glm::quat orientation, glm::vec3 scale, int seed);
    void init() override;

private:
    int spawnSeed;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_CORAL_H
