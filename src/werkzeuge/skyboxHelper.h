//
// Created by Alexey Pravilov on 04/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SKYBOXHELPER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SKYBOXHELPER_H
#include "glew.h"
#include <glm.hpp>

#include "visual/worldEnvironment.h"

inline float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

inline GLuint skyboxVAO, skyboxVBO;

inline void initSkybox() {
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

inline void RenderSkybox(const GLuint skyboxShader, const GLuint skyboxCubemapTexture, GLuint skyboxVAO, const Kamera& kamera, float time) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(skyboxShader);

    Kern::setUniform(skyboxShader, "projection", kamera.getProjectionMatrix());
    Kern::setUniform(skyboxShader, "view", kamera.getViewMatrix());
    Kern::setUniform(skyboxShader, "cameraPos", kamera.transform.position);
    Kern::setUniform(skyboxShader, "time", time);

    const EnvParameters env = (WorldEnvironment::activeEnv != nullptr)
                        ? WorldEnvironment::activeEnv->params
                        : EnvParameters();

    Kern::setUniform(skyboxShader, "u_fogColor", env.fogColor);
    Kern::setUniform(skyboxShader, "u_sunDirection", env.sunDirection);
    Kern::setUniform(skyboxShader, "u_heightFogColor", env.heightFogColor);
    Kern::setUniform(skyboxShader, "u_heightFogMin", env.heightFogMin);
    Kern::setUniform(skyboxShader, "u_heightFogMax", env.heightFogMax);
    Kern::setUniform(skyboxShader, "u_baseFogDensity", env.fogDensity);

    Kern::setUniform(skyboxShader, "u_depthDimmingEnabled", env.depthDimmingEnabled);
    Kern::setUniform(skyboxShader, "u_depthDimmingCoefficient", env.depthDimmingCoefficient);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapTexture);
    Kern::setUniform(skyboxShader, "skybox", 0);
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SKYBOXHELPER_H
