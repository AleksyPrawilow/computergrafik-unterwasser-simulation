#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_BLOOM_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_BLOOM_H

#include "glew.h"

class Bloom {
public:
    void init(int width, int height);
    void resize(int width, int height);
    void cleanup();

    void beginCapture();
    void endCaptureAndProcess(float threshold, float intensity, bool enabled);

private:
    void createFBOs(int width, int height);
    void destroyFBOs();
    void initQuad();

    GLuint sceneFBO = 0;
    GLuint sceneColorTex = 0;
    GLuint sceneBloomTex = 0;
    GLuint sceneDepthRBO = 0;

    GLuint pingpongFBO[2] = {0, 0};
    GLuint pingpongTex[2] = {0, 0};

    GLuint quadVAO = 0;
    GLuint quadVBO = 0;

    GLuint extractShader = 0;
    GLuint blurShader = 0;
    GLuint compositeShader = 0;

    int fbWidth = 0;
    int fbHeight = 0;

    static constexpr int BLUR_PASSES = 5;
};

#endif
