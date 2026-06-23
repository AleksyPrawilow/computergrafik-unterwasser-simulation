//
// Created by Alexey Pravilov on 19/06/2026.
//
#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/quaternion.hpp>

struct GodotTransform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    GodotTransform(glm::vec3 p, const glm::quat r, const glm::vec3 s)
        : position(p), rotation(r), scale(s) {}
};

inline std::unordered_map<std::string, std::vector<GodotTransform>> getGodotSceneData() {
    return {
            { "oceanbed", {
                GodotTransform(
                    glm::vec3(0.000000f, 0.000000f, 0.000000f),
                    glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                    glm::vec3(1.000000f, 1.000000f, 1.000000f)
                )
            } },
            { "rock", {
                GodotTransform(
                    glm::vec3(-514.793460f, -31.381348f, -179.766700f),
                    glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                    glm::vec3(91.810000f, 91.810000f, 91.810000f)
                ),
                GodotTransform(
                    glm::vec3(-606.585400f, -44.617294f, -349.051940f),
                    glm::quat(0.834492f, -0.347126f, 0.164356f, 0.395113f),
                    glm::vec3(184.850010f, 184.850013f, 184.850015f)
                ),
                GodotTransform(
                    glm::vec3(-542.000000f, -22.000000f, -147.000000f),
                    glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                    glm::vec3(30.000000f, 30.000000f, 30.000000f)
                ),
                GodotTransform(
                    glm::vec3(-610.000000f, -23.000000f, -183.000000f),
                    glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                    glm::vec3(30.000000f, 30.000000f, 30.000000f)
                ),
                GodotTransform(
                    glm::vec3(-623.000000f, -29.000000f, -263.000000f),
                    glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                    glm::vec3(30.000000f, 30.000000f, 30.000000f)
                )
            } }
    };
}