//
// Created by Alexey Pravilov on 04/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SKYBOXHELPER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SKYBOXHELPER_H
#include "glew.h"

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

inline void RenderSkybox(const GLuint skyboxShader, const GLuint skyboxCubemapTexture, const GLuint VAO, const Kamera& kamera) {
    // 1. Change depth function to GL_LEQUAL (less-than-or-equal)
    // By default, OpenGL uses GL_LESS. Because our skybox depth is exactly 1.0 (furthest),
    // we need LEQUAL so it can pass the depth test against a cleared depth buffer (which is also 1.0).
    glDepthFunc(GL_LEQUAL);

    glUseProgram(skyboxShader);

    // Pass view and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(kamera.getProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(kamera.getViewMatrix()));
    glUniform3fv(glGetUniformLocation(skyboxShader, "cameraPos"), 1, glm::value_ptr(kamera.transform.position));

    if (const GLint timeLocation = glGetUniformLocation(skyboxShader, "time"); timeLocation != -1) {
     glUniform1f(timeLocation, static_cast<GLfloat>(glfwGetTime()));
    }

    // Bind Cubemap Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapTexture);
    glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);

    // Draw the cube
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Reset the depth function back to normal
    glDepthFunc(GL_LESS);
}

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SKYBOXHELPER_H
