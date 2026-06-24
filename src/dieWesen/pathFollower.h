#pragma once
#include "werkzeuge/wesen.h"
#include "path3d.h"

class PathFollower : public Wesen {
public:
    Path3D* targetPath = nullptr;
    float moveSpeed = 5.0f;

    float rotationOffsetY = 0.0f;

    int currentNodeIndex = 0;

    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
};