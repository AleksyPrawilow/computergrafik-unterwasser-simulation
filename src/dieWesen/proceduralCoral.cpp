//
// Created by Alexey Pravilov on 25/06/2026.
//

#include "proceduralCoral.h"
#include <cmath>
#include <random>
#include <gtx/quaternion.hpp>

using namespace Kern;

void ProceduralCoral::generateBranch(
    const glm::vec3& start,
    const glm::vec3& direction,
    float length,
    float radius,
    int generation,
    int maxGenerations,
    std::vector<CoralVertex>& vertices,
    std::vector<unsigned int>& indices,
    AABB& outAABB
) {
    glm::vec3 end = start + direction * length;

    constexpr int radialSegments = 8;
    constexpr int heightSegments = 4;
    unsigned int startVertexIndex = static_cast<unsigned int>(vertices.size());

    // Construct an orthogonal coordinate frame perpendicular to the branch direction
    glm::vec3 up = (std::abs(direction.y) < 0.99f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    glm::vec3 localUp = glm::cross(right, direction);

    for (int h = 0; h <= heightSegments; h++) {
        float t = static_cast<float>(h) / static_cast<float>(heightSegments);
        glm::vec3 center = glm::mix(start, end, t);

        // Taper the branch slightly towards the tip
        float currentRadius = radius * (1.0f - t * 0.40f);

        for (int r = 0; r <= radialSegments; r++) {
            float angle = (static_cast<float>(r) / static_cast<float>(radialSegments)) * 2.0f * 3.14159265f;

            // --- FIX: ADD PROCEDURAL POLYP BUMPS ---
            // Adding a high-frequency sine/cosine wave perturbation to the radial offset
            // deforms the smooth cylinders into rough, bumpy, and organic coral surfaces.
            float polypNoise = (std::sin(angle * 5.0f) * std::cos(center.y * 4.0f)) * currentRadius * 0.28f;
            glm::vec3 radialOffset = (right * std::cos(angle) + localUp * std::sin(angle)) * (currentRadius + polypNoise);

            CoralVertex vertex;
            vertex.position = center + radialOffset;
            vertex.normal = glm::normalize(radialOffset);
            vertex.texCoord = glm::vec2(static_cast<float>(r) / radialSegments, t);

            vertices.push_back(vertex);

            // Expand the calculated local AABB bounds
            outAABB.min = glm::min(outAABB.min, vertex.position);
            outAABB.max = glm::max(outAABB.max, vertex.position);
        }
    }

    // Stitch the cylinder indices (triangle strip style)
    for (int h = 0; h < heightSegments; h++) {
        for (int r = 0; r < radialSegments; r++) {
            unsigned int curr = startVertexIndex + h * (radialSegments + 1) + r;
            unsigned int next = curr + 1;
            unsigned int aboveCurr = curr + (radialSegments + 1);
            unsigned int aboveNext = aboveCurr + 1;

            indices.push_back(curr);
            indices.push_back(next);
            indices.push_back(aboveNext);

            indices.push_back(curr);
            indices.push_back(aboveNext);
            indices.push_back(aboveCurr);
        }
    }

    // 2. Recursively split into child branches
    if (generation < maxGenerations) {
        std::mt19937 gen(static_cast<unsigned int>(start.x * 1000.0f + start.y * 10.0f + start.z));

        // FIX: Tightened the branching angles to 10-18 degrees
        // This forces the coral to grow upwards in tight, spiky clusters
        std::uniform_real_distribution<float> angleDist(10.0f, 18.0f);
        std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);

        // Branch 1
        float pitch1 = glm::radians(angleDist(gen));
        float yaw1 = glm::radians(rotDist(gen));
        glm::quat q1 = glm::angleAxis(yaw1, direction) * glm::angleAxis(pitch1, right);
        glm::vec3 newDir1 = glm::normalize(q1 * direction);

        // Branch 2 (Mirrored split)
        float pitch2 = glm::radians(angleDist(gen));
        float yaw2 = yaw1 + glm::radians(180.0f);
        glm::quat q2 = glm::angleAxis(yaw2, direction) * glm::angleAxis(pitch2, right);
        glm::vec3 newDir2 = glm::normalize(q2 * direction);

        float childLength = length * 0.78f; // Keeps branches longer
        float childRadius = radius * 0.65f;

        generateBranch(end, newDir1, childLength, childRadius, generation + 1, maxGenerations, vertices, indices, outAABB);
        generateBranch(end, newDir2, childLength, childRadius, generation + 1, maxGenerations, vertices, indices, outAABB);
    }
}

RenderContext ProceduralCoral::generate(int seed, AABB& outAABB) {
    std::vector<CoralVertex> vertices;
    std::vector<unsigned int> indices;

    outAABB.min = glm::vec3(1e30f);
    outAABB.max = glm::vec3(-1e30f);

    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> numTubesDist(3.0f, 6.0f);  // 3 to 5 chimneys in a cluster
    std::uniform_real_distribution<float> heightDist(1.5f, 3.2f);    // Randomized heights
    std::uniform_real_distribution<float> radiusDist(0.18f, 0.28f);   // Tube thicknesses

    int numTubes = static_cast<int>(numTubesDist(gen));

    constexpr int radialSegments = 8;
    constexpr int heightSegments = 6;

    for (int t = 0; t < numTubes; t++) {
        unsigned int tubeStart = static_cast<unsigned int>(vertices.size());

        // 1. Distribute tube bases in a tight circle around the cluster center
        float angleOffset = (static_cast<float>(t) / numTubes) * 2.0f * 3.14159265f;
        float clusterRadius = 0.35f;
        glm::vec3 startPos(std::cos(angleOffset) * clusterRadius, 0.0f, std::sin(angleOffset) * clusterRadius);

        // Flare the chimneys slightly outwards from the center of the cluster
        glm::vec3 direction = glm::normalize(glm::vec3(std::cos(angleOffset) * 0.15f, 1.0f, std::sin(angleOffset) * 0.15f));

        float height = heightDist(gen);
        float radius = radiusDist(gen);
        glm::vec3 end = startPos + direction * height;

        // Construct coordinate frame perpendicular to the growth direction
        glm::vec3 up = (std::abs(direction.y) < 0.99f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(direction, up));
        glm::vec3 localUp = glm::cross(right, direction);

        for (int h = 0; h <= heightSegments; h++) {
            float tFactor = static_cast<float>(h) / heightSegments;
            glm::vec3 center = glm::mix(startPos, end, tFactor);

            // Tube Sponges expand slightly towards the top, then taper sharply at the rim
            float currentRadius = radius;
            if (tFactor < 0.85f) {
                currentRadius = glm::mix(radius * 0.8f, radius * 1.15f, tFactor / 0.85f);
            } else {
                currentRadius = glm::mix(radius * 1.15f, radius * 0.70f, (tFactor - 0.85f) / 0.15f);
            }

            for (int r = 0; r <= radialSegments; r++) {
                float angle = (static_cast<float>(r) / radialSegments) * 2.0f * 3.14159265f;

                // Add porous, organic sponge-basalt bumps
                float spongeBumps = (std::sin(angle * 4.0f) * std::cos(center.y * 3.0f)) * currentRadius * 0.15f;
                glm::vec3 radialOffset = (right * std::cos(angle) + localUp * std::sin(angle)) * (currentRadius + spongeBumps);

                CoralVertex v;
                v.position = center + radialOffset;
                v.normal = glm::normalize(radialOffset);
                v.texCoord = glm::vec2(static_cast<float>(r) / radialSegments, tFactor);

                // If at the very top rim (the chimney opening), push the normal slightly upward
                if (h == heightSegments) {
                    v.normal = glm::normalize(v.normal + glm::vec3(0.0f, 0.6f, 0.0f));
                }

                vertices.push_back(v);

                outAABB.min = glm::min(outAABB.min, v.position);
                outAABB.max = glm::max(outAABB.max, v.position);
            }
        }

        // Stitch the hollow rocky chimney
        for (int h = 0; h < heightSegments; h++) {
            for (int r = 0; r < radialSegments; r++) {
                unsigned int curr = tubeStart + h * (radialSegments + 1) + r;
                unsigned int next = curr + 1;
                unsigned int aboveCurr = curr + (radialSegments + 1);
                unsigned int aboveNext = aboveCurr + 1;

                indices.push_back(curr);
                indices.push_back(next);
                indices.push_back(aboveNext);
                indices.push_back(curr);
                indices.push_back(aboveNext);
                indices.push_back(aboveCurr);
            }
        }
    }

    // --- UPLOAD PROC MESHDATA TO GPU VAO/VBOs ---
    RenderContext rc;
    rc.size = static_cast<int>(indices.size());

    glGenVertexArrays(1, &rc.vertexArray);
    glBindVertexArray(rc.vertexArray);

    glGenBuffers(1, &rc.vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rc.vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &rc.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rc.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CoralVertex), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CoralVertex), (void*)offsetof(CoralVertex, position));
    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CoralVertex), (void*)offsetof(CoralVertex, normal));
    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CoralVertex), (void*)offsetof(CoralVertex, texCoord));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return rc;
}