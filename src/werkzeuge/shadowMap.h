#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SHADOWMAP_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SHADOWMAP_H

#include "glew.h"
#include <glm.hpp>

class ShadowMap {
public:
    void init(int resolution = 2048);
    void cleanup();

    void updateDirectional(const glm::vec3& lightDir, const glm::vec3& sceneCenter, float sceneRadius);

    void beginPass();
    void endPass();

    GLuint getDepthTexture() const { return depthTexture; }
    glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }

private:
    GLuint fbo = 0;
    GLuint depthTexture = 0;
    int resolution = 2048;
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

    GLint savedViewport[4] = {};
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SHADOWMAP_H
