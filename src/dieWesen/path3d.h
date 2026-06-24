//
// Created by mikol on 23.06.2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PATH3D_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_PATH3D_H

#include "werkzeuge/wesen.h"
#include <vector>

struct PathFrame {
    glm::vec3 position;
    glm::vec3 tangent;
    glm::vec3 normal;
    glm::vec3 binormal;
};

class Path3D : public Wesen {
public:
    std::vector<PathFrame> frames;

    void generateFigureEight(float sizeX, float sizeY, float sizeZ, int numPoints = 100);
    void generateCircle(float radius, int numPoints = 100);

    void generateFromSpline(const std::vector<glm::vec3>& controlPoints, int segmentsPerCurve = 20, bool closedLoop = true);

private:
    void calculateParallelTransportFrames(const std::vector<glm::vec3>& points);
};


#endif
