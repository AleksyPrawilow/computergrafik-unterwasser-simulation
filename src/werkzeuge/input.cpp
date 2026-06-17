//
// Created by Alexey Pravilov on 11/06/2026.
//

#include "input.h"

namespace Input {
    static std::array<bool, GLFW_KEY_LAST + 1> currentStates{};
    static std::array<bool, GLFW_KEY_LAST + 1> previousStates{};
}

void Input::init(GLFWwindow* window) {
    glfwSetKeyCallback(window, keyCallback);
}

void Input::update() {
    previousStates = currentStates;
    glfwPollEvents();
}

bool Input::isKeyPressed(int key) {
    if (key < GLFW_KEY_SPACE || key > GLFW_KEY_LAST) return false;
    return currentStates[key];
}

bool Input::isKeyJustPressed(const int key) {
    if (key < GLFW_KEY_SPACE || key > GLFW_KEY_LAST) return false;
    return currentStates[key] && !previousStates[key];
}

bool Input::isKeyJustReleased(const int key) {
    if (key < GLFW_KEY_SPACE || key > GLFW_KEY_LAST) return false;
    return !currentStates[key] && previousStates[key];
}

void Input::keyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods) {
    if (key < GLFW_KEY_SPACE || key > GLFW_KEY_LAST) return;

    if (action == GLFW_PRESS) {
        currentStates[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        currentStates[key] = false;
    }
}
