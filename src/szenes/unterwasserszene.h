//
// Created by Alexey Pravilov on 02/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENE_H

#include "../dieWesen/uboot.h"

//
// Created by Alexey Pravilov on 02/06/2026.
//

void init(GLFWwindow* window);
void renderLoop(GLFWwindow* window);
void shutdown(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENE_H
