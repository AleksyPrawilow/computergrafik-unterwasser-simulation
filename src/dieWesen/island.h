//
// Created by Alexey Pravilov on 17/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ISLAND_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ISLAND_H
#include "werkzeuge/wesen.h"
#include <vector>


class Island : public Wesen {
public:
    void init() override;
    float getHeight(float x, float z) const;
private:
    std::vector<glm::vec3> vertices;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ISLAND_H
