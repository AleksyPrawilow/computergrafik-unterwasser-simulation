//
// Created by Alexey Pravilov on 08/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OCEANFLOOR_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OCEANFLOOR_H
#include "werkzeuge/wesen.h"
#include <unordered_map>

class OceanFloor : public Wesen {
public:
    void init() override;
    float getHeight(float x, float z) const;
private:
    std::vector<glm::vec3> vertices;

    float gridCellSize = 100.0f;
    std::unordered_map<int64_t, std::vector<int>> grid;
    int64_t gridKey(int gx, int gz) const { return (int64_t(gx) << 32) | (int64_t(gz) & 0xFFFFFFFF); }
    void buildGrid();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OCEANFLOOR_H
