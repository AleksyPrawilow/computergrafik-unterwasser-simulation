//
// Created by Alexey Pravilov on 17/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ISLAND_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ISLAND_H
#include "werkzeuge/wesen.h"
#include <unordered_map>
#include <vector>


class Island : public Wesen {
public:
    void init() override;
    float getHeight(float x, float z) const;
    void graben(glm::vec3 weltPos, float radius, float tiefe);
private:
    std::vector<glm::vec3> vertices;

    float gridCellSize = 5.0f;
    std::unordered_map<int64_t, std::vector<int>> grid;
    int64_t gridKey(int gx, int gz) const { return (int64_t(gx) << 32) | (int64_t(gz) & 0xFFFFFFFF); }
    void buildGrid();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_ISLAND_H
