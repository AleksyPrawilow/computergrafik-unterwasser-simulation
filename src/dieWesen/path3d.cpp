#include "path3d.h"
#include "gtc/constants.hpp"

void Path3D::generateFigureEight(float sizeX, float sizeY, float sizeZ, int numPoints) {
    std::vector<glm::vec3> points;
    for (int i = 0; i < numPoints; i++) {
        float t = (float)i / (float)numPoints * glm::two_pi<float>();
        glm::vec3 pos = this->transform.position + glm::vec3(
            sin(t) * sizeX,
            sin(t * 2.0f) * sizeY,
            cos(t) * sizeZ
        );
        points.push_back(pos);
    }
    calculateParallelTransportFrames(points);
}

void Path3D::generateCircle(float radius, int numPoints) {
    std::vector<glm::vec3> points;
    for (int i = 0; i < numPoints; i++) {
        float t = (float)i / (float)numPoints * glm::two_pi<float>();
        glm::vec3 pos = this->transform.position + glm::vec3(
            sin(t) * radius,
            0.0f,
            cos(t) * radius
        );
        points.push_back(pos);
    }
    calculateParallelTransportFrames(points);
}

void Path3D::calculateParallelTransportFrames(const std::vector<glm::vec3>& points) {
    if (points.size() < 2) return;
    frames.resize(points.size());

    for (size_t i = 0; i < points.size(); i++) {
        frames[i].position = points[i];
        glm::vec3 nextPoint = points[(i + 1) % points.size()];
        frames[i].tangent = glm::normalize(nextPoint - points[i]);
    }

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    frames[0].normal = glm::normalize(glm::cross(frames[0].tangent, glm::cross(up, frames[0].tangent)));
    frames[0].binormal = glm::cross(frames[0].tangent, frames[0].normal);

    for (size_t i = 0; i < points.size() - 1; i++) {
        glm::vec3 t0 = frames[i].tangent;
        glm::vec3 t1 = frames[i + 1].tangent;

        glm::quat rot = glm::rotation(t0, t1);

        frames[i + 1].normal = glm::normalize(rot * frames[i].normal);
        frames[i + 1].binormal = glm::cross(frames[i + 1].tangent, frames[i + 1].normal);
    }
}