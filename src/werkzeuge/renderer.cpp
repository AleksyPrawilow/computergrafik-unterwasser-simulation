//
// Created by Alexey Pravilov on 01/06/2026.
//

#include "renderer.h"

#include "lightManager.h"
#include "textur.h"
#include "gtc/type_ptr.inl"

void Renderer::init() {
    defaultNormal = Kern::LoadTexture("assets/textures/default_normal.png");
}

void Renderer::render (const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos)
{
    if (e.hasCustomRender()) {
        e.customRender(view, projection);
        return;
    }

    const Material& m = e.material;

    glUseProgram(m.shader);

    glm::mat4 model = e.getGlobalModelMatrix();
    glm::mat4 mvp = projection * view * model;

    e.prepareUniforms();

    glUniformMatrix4fv(
        glGetUniformLocation(m.shader, "transformation"),
        1,
        GL_FALSE,
        &mvp[0][0]
    );

    glUniformMatrix4fv(
        glGetUniformLocation(m.shader, "modelMatrix"),
        1,
        GL_FALSE,
        &model[0][0]
    );

    glUniform3fv(
        glGetUniformLocation(m.shader, "cameraPos"),
        1,
        glm::value_ptr(cameraPos)
    );

    if (const GLint timeLocation = glGetUniformLocation(m.shader, "time"); timeLocation != -1) {
        glUniform1f(timeLocation, static_cast<float>(glfwGetTime()));
    }

    Kern::SetActiveTexture(m.albedo, "colorTexture", m.shader, 0);
    if (m.normal != 0) {
        Kern::SetActiveTexture(m.normal, "normalMap", m.shader, 1);
    } else {
        Kern::SetActiveTexture(defaultNormal, "normalMap", m.shader, 1);
    }

    if (m.roughness)
        Kern::SetActiveTexture(m.roughness, "roughnessMap", m.shader, 2);

    if (m.metallic)
        Kern::SetActiveTexture(m.metallic, "metallicMap", m.shader, 3);

    const auto& pointLights = LightManager::getInstance().getPointLights();
    const auto& spotLights = LightManager::getInstance().getSpotLights();

    for (int i = 0; i < 4; ++i) {
        std::string base = "pointLights[" + std::to_string(i) + "].";
        if (i < pointLights.size()) {
            glUniform3fv(glGetUniformLocation(m.shader, (base + "position").c_str()), 1, glm::value_ptr(pointLights[i]->position));
            glUniform3fv(glGetUniformLocation(m.shader, (base + "color").c_str()), 1, glm::value_ptr(pointLights[i]->color));
            glUniform1f(glGetUniformLocation(m.shader, (base + "intensity").c_str()), pointLights[i]->intensity);
        } else {
            glUniform1f(glGetUniformLocation(m.shader, (base + "intensity").c_str()), 0.0f);
        }
    }

    for (int i = 0; i < 2; ++i) {
        std::string base = "spotLights[" + std::to_string(i) + "].";
        if (i < spotLights.size()) {
            glUniform3fv(glGetUniformLocation(m.shader, (base + "position").c_str()), 1, glm::value_ptr(spotLights[i]->position));
            glUniform3fv(glGetUniformLocation(m.shader, (base + "direction").c_str()), 1, glm::value_ptr(spotLights[i]->direction));
            glUniform3fv(glGetUniformLocation(m.shader, (base + "color").c_str()), 1, glm::value_ptr(spotLights[i]->color));
            glUniform1f(glGetUniformLocation(m.shader, (base + "intensity").c_str()), spotLights[i]->intensity);
            glUniform1f(glGetUniformLocation(m.shader, (base + "cutOff").c_str()), spotLights[i]->cutOff);
            glUniform1f(glGetUniformLocation(m.shader, (base + "outerCutOff").c_str()), spotLights[i]->outerCutOff);
        } else {
            glUniform1f(glGetUniformLocation(m.shader, (base + "intensity").c_str()), 0.0f);
        }
    }

    Kern::DrawContext(e.mesh);

    glUseProgram(0);

    e.postRender(this, view, projection, cameraPos);
}
