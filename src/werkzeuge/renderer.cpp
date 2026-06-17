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
    if (!e.visible) {
        return;
    }

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

    if (const GLint invModelLoc = Kern::getUniformLocation(m.shader, "inverseModelMatrix"); invModelLoc != -1) {
        glm::mat4 invModel = glm::inverse(model);
        glUniformMatrix4fv(invModelLoc, 1, GL_FALSE, glm::value_ptr(invModel));
    }

    auto [
        sunDirection,
        sunColor,
        sunEnergy,
        ambientColor,
        ambientEnergy,
        fogEnabled,
        fogColor,
        fogDensity,
        heightFogEnabled,
        heightFogColor,
        heightFogMin,
        heightFogMax,
        causticsEnabled,
        causticsColor,
        causticsScale,
        causticsIntensity,
        depthDimmingEnabled,
        depthDimmingCoefficient
        ] = (WorldEnvironment::activeEnv != nullptr)
                        ? WorldEnvironment::activeEnv->params
                        : EnvParameters();

    // 1. Sun (Directional Light)
    Kern::setUniform(m.shader, "u_sunDirection",  sunDirection);
    Kern::setUniform(m.shader, "u_sunColor",      sunColor);
    Kern::setUniform(m.shader, "u_sunEnergy",     sunEnergy);

    // 2. Ambient Light
    Kern::setUniform(m.shader, "u_ambientColor",  ambientColor);
    Kern::setUniform(m.shader, "u_ambientEnergy", ambientEnergy);

    // 3. Distance Fog
    Kern::setUniform(m.shader, "u_fogEnabled",     fogEnabled);
    Kern::setUniform(m.shader, "u_fogColor",       fogColor);
    Kern::setUniform(m.shader, "u_baseFogDensity", fogDensity);

    // 4. Height/Depth Fog
    Kern::setUniform(m.shader, "u_heightFogEnabled", heightFogEnabled);
    Kern::setUniform(m.shader, "u_heightFogColor",   heightFogColor);
    Kern::setUniform(m.shader, "u_heightFogMin",     heightFogMin);
    Kern::setUniform(m.shader, "u_heightFogMax",     heightFogMax);

    // 5. Projected Caustics
    Kern::setUniform(m.shader, "u_causticsEnabled",   causticsEnabled);
    Kern::setUniform(m.shader, "u_causticsColor",     causticsColor);
    Kern::setUniform(m.shader, "u_causticsScale",     causticsScale);
    Kern::setUniform(m.shader, "u_causticsIntensity", causticsIntensity);

    // 6. Global Matrices & Camera Position
    Kern::setUniform(m.shader, "transformation", mvp);
    Kern::setUniform(m.shader, "modelMatrix",    model);
    Kern::setUniform(m.shader, "cameraPos",      cameraPos);

    // 7. Depth dimming
    Kern::setUniform(m.shader, "u_depthDimmingEnabled", depthDimmingEnabled);
    Kern::setUniform(m.shader, "u_depthDimmingCoefficient", depthDimmingCoefficient);

    Kern::setUniform(m.shader, "transformation", mvp);
    Kern::setUniform(m.shader, "modelMatrix",    model);
    Kern::setUniform(m.shader, "cameraPos",      cameraPos);

    Kern::setUniform(m.shader, "time", static_cast<float>(glfwGetTime()));

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
            Kern::setUniform(m.shader, (base + "position").c_str(), pointLights[i]->position);
            Kern::setUniform(m.shader, (base + "color").c_str(), pointLights[i]->color);
            Kern::setUniform(m.shader, (base + "intensity").c_str(), pointLights[i]->intensity);
        } else {
            Kern::setUniform(m.shader, (base + "intensity").c_str(), 0.0f);
        }
    }

    const auto& spotLights = LightManager::getInstance().getSpotLights();
    for (int i = 0; i < 2; ++i) {
        std::string base = "spotLights[" + std::to_string(i) + "].";
        if (i < spotLights.size()) {
            Kern::setUniform(m.shader, (base + "position").c_str(), spotLights[i]->position);
            Kern::setUniform(m.shader, (base + "color").c_str(), spotLights[i]->color);
            Kern::setUniform(m.shader, (base + "intensity").c_str(), spotLights[i]->intensity);
            Kern::setUniform(m.shader, (base + "direction").c_str(), spotLights[i]->direction);
            Kern::setUniform(m.shader, (base + "cutOff").c_str(), spotLights[i]->cutOff);
            Kern::setUniform(m.shader, (base + "outerCutOff").c_str(), spotLights[i]->outerCutOff);
        } else {
            Kern::setUniform(m.shader, (base + "intensity").c_str(), 0.0f);
        }
    }

    if (const GLint skyboxLocation = Kern::getUniformLocation(m.shader, "skybox"); skyboxLocation != -1) {
        glUniform1i(skyboxLocation, 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    }

    Kern::DrawContext(e.mesh);
    glUseProgram(0);
}
