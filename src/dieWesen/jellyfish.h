#pragma once

#include "../werkzeuge/wesen.h"
#include "../werkzeuge/kamera.h"

class Jellyfish : public Wesen {
public:
    void init() override;
    void draw(Kamera& kamera, unsigned int shaderId);
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

};