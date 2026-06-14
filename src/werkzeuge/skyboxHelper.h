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
    // Positions
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

// Generate and bind VAO/VBO inside your initialization code
inline void initSkybox() {
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

inline void RenderSkybox(GLuint skyboxShader, GLuint skyboxCubemapTexture, GLuint skyboxVAO, const Kamera& kamera, float time) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(skyboxShader);

    // Pass matrices
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(kamera.getProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(kamera.getViewMatrix()));

    // --- 1. SEND cameraPos TO SKYBOX FRAGMENT SHADER ---
    glUniform3fv(glGetUniformLocation(skyboxShader, "cameraPos"), 1, glm::value_ptr(kamera.transform.position));

    // --- 2. SEND time TO SKYBOX FRAGMENT SHADER ---
    glUniform1f(glGetUniformLocation(skyboxShader, "time"), time);

    EnvParameters env = (WorldEnvironment::activeEnv != nullptr)
                        ? WorldEnvironment::activeEnv->params
                        : EnvParameters();

    glUniform3fv(glGetUniformLocation(skyboxShader, "u_fogColor"), 1, glm::value_ptr(env.fogColor));
    glUniform3fv(glGetUniformLocation(skyboxShader, "u_sunDirection"), 1, glm::value_ptr(env.sunDirection));
    glUniform3fv(glGetUniformLocation(skyboxShader, "u_heightFogColor"), 1, glm::value_ptr(env.heightFogColor));
    glUniform1f(glGetUniformLocation(skyboxShader, "u_heightFogMin"), env.heightFogMin);
    glUniform1f(glGetUniformLocation(skyboxShader, "u_heightFogMax"), env.heightFogMax);
    glUniform1f(glGetUniformLocation(skyboxShader, "u_baseFogDensity"), env.fogDensity);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapTexture);
    glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);

    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SKYBOXHELPER_H
