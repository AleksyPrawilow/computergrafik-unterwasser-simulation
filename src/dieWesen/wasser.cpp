//
// Created by Alexey Pravilov on 04/06/2026.
//

#include "wasser.h"

#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Wasser::init() {
    loadModel("assets/models/wasser.obj");
    material.shader = ShaderManager::getInstance().loadShader(
        "wasser",
        "assets/shaders/wasser.vert",
        "assets/shaders/wasser.frag"
    );
    material.normal = Kern::LoadTexture("assets/textures/water_normal.png");
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
}

void Wasser::prepareUniforms() const {
    const GLuint s = material.shader;

    if (const GLint timeLocation = glGetUniformLocation(material.shader, "time"); timeLocation != -1) {
        glUniform1f(timeLocation, static_cast<float>(glfwGetTime()));
    }

    // 2. Initialize Wave 0 (Must not have a wavelength of 0)
    glUniform2f(glGetUniformLocation(s, "waves[0].direction"), 1.0f, 0.1f);
    glUniform1f(glGetUniformLocation(s, "waves[0].amplitude"), 0.25f);
    glUniform1f(glGetUniformLocation(s, "waves[0].steepness"), 0.5f);
    glUniform1f(glGetUniformLocation(s, "waves[0].wavelength"), 12.0f);
    glUniform1f(glGetUniformLocation(s, "waves[0].speed"), 1.5f);

    // 3. Initialize Wave 1
    glUniform2f(glGetUniformLocation(s, "waves[1].direction"), 0.2f, 1.0f);
    glUniform1f(glGetUniformLocation(s, "waves[1].amplitude"), 0.15f);
    glUniform1f(glGetUniformLocation(s, "waves[1].steepness"), 0.5f);
    glUniform1f(glGetUniformLocation(s, "waves[1].wavelength"), 6.0f);
    glUniform1f(glGetUniformLocation(s, "waves[1].speed"), 1.0f);

    // 4. Initialize Wave 2
    glUniform2f(glGetUniformLocation(s, "waves[2].direction"), -0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(s, "waves[2].amplitude"), 0.08f);
    glUniform1f(glGetUniformLocation(s, "waves[2].steepness"), 0.5f);
    glUniform1f(glGetUniformLocation(s, "waves[2].wavelength"), 3.0f);
    glUniform1f(glGetUniformLocation(s, "waves[2].speed"), 0.8f);
}


