//
// Created by Alexey Pravilov on 08/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OCEANFLOOR_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OCEANFLOOR_H
#include "werkzeuge/wesen.h"


class OceanFloor : public Wesen {
public:
    void init() override;
    float getHeight(const float x, const float z) const;
private:
    std::vector<glm::vec3> vertices;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OCEANFLOOR_H
