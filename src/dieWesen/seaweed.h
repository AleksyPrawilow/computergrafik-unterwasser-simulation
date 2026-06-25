//
// Created by Alexey Pravilov on 25/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SEAWEED_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SEAWEED_H
#include "prop.h"


class Seaweed : public Prop {
public:
    Seaweed(glm::vec3 position, glm::quat orientation, glm::vec3 scale) : Prop(position, orientation, scale) {};
    void init() override;
    void prepareUniforms() const override;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SEAWEED_H
