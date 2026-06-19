//
// Created by Alexey Pravilov on 19/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ROCK_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ROCK_H
#include "prop.h"

class Rock : public Prop {
public:
    Rock(const glm::vec3 position, const glm::quat orientation, const glm::vec3 scale) : Prop(position, orientation, scale) {};
    void init() override;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ROCK_H
