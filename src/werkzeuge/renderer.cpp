//
// Created by Alexey Pravilov on 01/06/2026.
//

#include "renderer.h"
#include "visual/lightManager.h"
#include "shaderManager.h"
#include "textur.h"
#include "wesen.h"
#include "renderWerkzeuge.h"
#include "gtc/type_ptr.inl"
#include <algorithm>

#include "visual/worldEnvironment.h"

extern GLuint cubemapTexture;

void Renderer::init() {
    defaultNormal = Kern::LoadTexture("assets/textures/default_normal.png");
    defaultEmission = Kern::LoadTexture("assets/textures/default_emission.png");
    defaultOpacity = Kern::LoadTexture("assets/textures/default_opacity.png");
}

void Renderer::render(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    if (e.material.isUI) {
        uiQueue.push_back(&e);
    } else if (e.material.isTransparent) {
        transparentQueue.push_back(&e);
    } else {
        opaqueQueue.push_back(&e);
    }

    for (const Wesen* child : e.children) {
        render(*child, view, projection, cameraPos);
    }
}

void Renderer::drawOpaque(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) const {
    for (const Wesen* e : opaqueQueue) {
        drawElement(*e, view, projection, cameraPos);
    }
}

void Renderer::drawTransparent(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    if (transparentQueue.empty()) return;

    std::sort(transparentQueue.begin(), transparentQueue.end(), [&cameraPos](const Wesen* a, const Wesen* b) {
        const float distA = glm::distance(glm::vec3(a->getGlobalModelMatrix()[3]), cameraPos);
        const float distB = glm::distance(glm::vec3(b->getGlobalModelMatrix()[3]), cameraPos);
        return distA > distB;
    });

    Kern::SetBlendState(true);
    Kern::SetCullState(false);
    Kern::SetDepthWriteState(false);

    for (const Wesen* e : transparentQueue) {
        drawElement(*e, view, projection, cameraPos);
    }

    Kern::SetDepthWriteState(true);
    Kern::SetCullState(true);
    Kern::SetBlendState(false);
}

void Renderer::drawUI(const glm::mat4& view, const glm::mat4& projection) const {
    if (uiQueue.empty()) return;

    Kern::Set2DRenderState(true);

    for (const Wesen* e : uiQueue) {
        drawElement(*e, view, projection, glm::vec3(0.0f));
    }

    Kern::Set2DRenderState(false);
}

void Renderer::drawElement(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) const {
    if (e.hasCustomRender()) {
        e.customRender(view, projection);
        return;
    }

    const Material& m = e.material;
    glUseProgram(m.shader);

    glm::mat4 model = e.getGlobalModelMatrix();
    glm::mat4 mvp = projection * view * model;

    e.prepareUniforms();

    EnvParameters env = (WorldEnvironment::activeEnv != nullptr)
                        ? WorldEnvironment::activeEnv->params
                        : EnvParameters();

    // 1. Sun (Directional Light)
    glUniform3fv(glGetUniformLocation(m.shader, "u_sunDirection"), 1, glm::value_ptr(env.sunDirection));
    glUniform3fv(glGetUniformLocation(m.shader, "u_sunColor"), 1, glm::value_ptr(env.sunColor));
    glUniform1f(glGetUniformLocation(m.shader, "u_sunEnergy"), env.sunEnergy);

    // 2. Ambient Light
    glUniform3fv(glGetUniformLocation(m.shader, "u_ambientColor"), 1, glm::value_ptr(env.ambientColor));
    glUniform1f(glGetUniformLocation(m.shader, "u_ambientEnergy"), env.ambientEnergy);

    // 3. Distance Fog
    glUniform1i(glGetUniformLocation(m.shader, "u_fogEnabled"), env.fogEnabled);
    glUniform3fv(glGetUniformLocation(m.shader, "u_fogColor"), 1, glm::value_ptr(env.fogColor));
    glUniform1f(glGetUniformLocation(m.shader, "u_baseFogDensity"), env.fogDensity);

    // 4. Height/Depth Fog
    glUniform1i(glGetUniformLocation(m.shader, "u_heightFogEnabled"), env.heightFogEnabled);
    glUniform3fv(glGetUniformLocation(m.shader, "u_heightFogColor"), 1, glm::value_ptr(env.heightFogColor));
    glUniform1f(glGetUniformLocation(m.shader, "u_heightFogMin"), env.heightFogMin);
    glUniform1f(glGetUniformLocation(m.shader, "u_heightFogMax"), env.heightFogMax);

    // 5. Projected Caustics
    glUniform1i(glGetUniformLocation(m.shader, "u_causticsEnabled"), env.causticsEnabled);
    glUniform3fv(glGetUniformLocation(m.shader, "u_causticsColor"), 1, glm::value_ptr(env.causticsColor));
    glUniform1f(glGetUniformLocation(m.shader, "u_causticsScale"), env.causticsScale);
    glUniform1f(glGetUniformLocation(m.shader, "u_causticsIntensity"), env.causticsIntensity);
    // -----------------------------------------------------------------

    glUniformMatrix4fv(glGetUniformLocation(m.shader, "transformation"), 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m.shader, "modelMatrix"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(m.shader, "cameraPos"), 1, glm::value_ptr(cameraPos));

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
    else
        Kern::SetActiveTexture(defaultEmission, "metallicMap", m.shader, 3);

    if (m.emission != 0) {
        Kern::SetActiveTexture(m.emission, "emissionMap", m.shader, 5);
    } else {
        Kern::SetActiveTexture(defaultEmission, "emissionMap", m.shader, 5);
    }

    if (m.opacity != 0) {
        Kern::SetActiveTexture(m.opacity, "opacityMap", m.shader, 7);
    } else {
        Kern::SetActiveTexture(defaultOpacity, "opacityMap", m.shader, 7);
    }

    const auto& pointLights = LightManager::getInstance().getPointLights();
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

    const auto& spotLights = LightManager::getInstance().getSpotLights();
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

    GLint skyboxLocation = glGetUniformLocation(m.shader, "skybox");
    if (skyboxLocation != -1) {
        glUniform1i(skyboxLocation, 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    }

    Kern::DrawContext(e.mesh);
    glUseProgram(0);
}
