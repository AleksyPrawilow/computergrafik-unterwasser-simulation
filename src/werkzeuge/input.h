//
// Created by Alexey Pravilov on 11/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INPUT_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INPUT_H
#include <array>
#include <GLFW/glfw3.h>

namespace Input {
    void init(GLFWwindow* window);
    void update();

    bool isKeyPressed(int key);
    bool isKeyJustPressed(int key);
    bool isKeyJustReleased(int key);

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
}

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INPUT_H
