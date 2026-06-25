//
// Created by Alexey Pravilov on 25/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LABSPROPS_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LABSPROPS_H

#include "unterwasserszeneProps.h"

inline std::unordered_map<std::string, std::vector<GodotTransform>> getLabsProps() {
    return {
        { "canister", {
            GodotTransform(
                glm::vec3(18.010078f, 2.556969f, 2.729604f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(2.000000f, 2.000000f, 2.000000f)
            )
        } },
        { "ubootpos", {
            GodotTransform(
                glm::vec3(10.497084f, 0.000000f, -12.441389f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(3.000000f, 3.000000f, 3.000000f)
            )
        } },
        { "bridge", {
            GodotTransform(
                glm::vec3(-9.160192f, 0.000000f, 0.000000f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(0.560000f, 0.560000f, 0.560000f)
            ),
            GodotTransform(
                glm::vec3(11.715777f, 0.000000f, 0.000000f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(0.560000f, 0.560000f, 0.560000f)
            ),
            GodotTransform(
                glm::vec3(3.839983f, 30.672104f, -9.407429f),
                glm::quat(0.974930f, 0.000000f, -0.222510f, 0.000000f),
                glm::vec3(0.560000f, 0.560000f, 0.560000f)
            ),
            GodotTransform(
                glm::vec3(-12.0f, 30.672104f, 3.631607f),
                glm::quat(0.974930f, 0.000000f, -0.222510f, 0.000000f),
                glm::vec3(0.560000f, 0.560000f, 0.560000f)
            ),
            GodotTransform(
                glm::vec3(22.033978f, 0.000000f, 0.000000f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(0.560000f, 0.560000f, 0.560000f)
            )
        } },
        { "bridge_open", {
            GodotTransform(
                glm::vec3(1.257554f, 0.000000f, 0.000000f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(0.560000f, 0.560000f, 0.560000f)
            )
        } },
        { "cave", {
            GodotTransform(
                glm::vec3(30.817050f, 0.000015f, -1.633166f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(25.000000f, 25.000000f, 25.000000f)
            )
        } },
        { "playerpos", {
            GodotTransform(
                glm::vec3(11.698412f, 3.185538f, -0.125142f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(1.000000f, 1.000000f, 1.000000f)
            )
        } },
        { "platform", {
            GodotTransform(
                glm::vec3(3.000000f, 31.800323f, -9.000000f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(0.680000f, 0.680000f, 0.680000f)
            )
        } },
        { "spaceship", {
            GodotTransform(
                glm::vec3(3.000000f, 33.000000f, -4.000000f),
                glm::quat(1.000000f, 0.000000f, 0.000000f, 0.000000f),
                glm::vec3(10.410000f, 10.410000f, 10.410000f)
            )
        } }
    };
}
#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LABSPROPS_H
