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
#include "himmelsboxWesen.h"

void Renderer::init() {
    defaultNormal = Kern::LoadTexture("assets/textures/default_normal.png");
    defaultEmission = Kern::LoadTexture("assets/textures/default_emission.png");
    defaultOpacity = Kern::LoadTexture("assets/textures/default_opacity.png");

    glGenBuffers(1, &envUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, envUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalEnvironmentData), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, envUBO);
}

void Renderer::render(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    if (!e.visible) {
        return;
    }

    // --- ACCURATE AABB FRUSTUM CULLING ---
    // Cull any 3D physical object containing actively loaded geometry meshes
    if (!e.material.isUI && e.hasMesh) {
        glm::mat4 model = e.getGlobalModelMatrix();
        glm::vec3 localMin = e.localAABB.min;
        glm::vec3 localMax = e.localAABB.max;

        // 1. Generate the 8 corner points of the local bounding box
        glm::vec3 corners[8] = {
            glm::vec3(localMin.x, localMin.y, localMin.z),
            glm::vec3(localMin.x, localMin.y, localMax.z),
            glm::vec3(localMin.x, localMax.y, localMin.z),
            glm::vec3(localMin.x, localMax.y, localMax.z),
            glm::vec3(localMax.x, localMin.y, localMin.z),
            glm::vec3(localMax.x, localMin.y, localMax.z),
            glm::vec3(localMax.x, localMax.y, localMin.z),
            glm::vec3(localMax.x, localMax.y, localMax.z)
        };

        glm::vec3 worldMin(1e30f);
        glm::vec3 worldMax(-1e30f);

        for (int i = 0; i < 8; i++) {
            glm::vec3 worldCorner = glm::vec3(model * glm::vec4(corners[i], 1.0f));
            worldMin = glm::min(worldMin, worldCorner);
            worldMax = glm::max(worldMax, worldCorner);
        }

        if (!frustum.isAABBInside(worldMin, worldMax)) {
            goto process_children;
        }

        if (showDebugAABBs) {
            debugAABBs.push_back({ worldMin, worldMax });
        }
    }

    if (dynamic_cast<const HimmelsboxWesen*>(&e) != nullptr) {
        himmelsboxQueue.push_back(&e);
    } else if (e.material.isUI) {
        uiQueue.push_back(&e);
    } else if (e.material.isTransparent) {
        transparentQueue.push_back(&e);
    } else {
        opaqueQueue.push_back(&e);
    }

process_children:
    for (const Wesen* child : e.children) {
        render(*child, view, projection, cameraPos);
    }
}

void Renderer::drawOpaque(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    for (const Wesen* e : opaqueQueue) {
        drawElement(*e, view, projection, cameraPos);
    }
}

void Renderer::drawHimmelsbox(const glm::mat4& view, const glm::mat4& projection) const {
    for (const Wesen* e : himmelsboxQueue) {
        e->customRender(view, projection);
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

void Renderer::drawUI(const glm::mat4& view, const glm::mat4& projection) {
    if (uiQueue.empty()) return;

    Kern::Set2DRenderState(true);

    for (const Wesen* e : uiQueue) {
        drawElement(*e, view, projection, glm::vec3(0.0f));
    }

    Kern::Set2DRenderState(false);
}

void Renderer::updateFrustum(const glm::mat4& view, const glm::mat4& projection) {
    frustum.update(projection * view);
}

void Renderer::sendEnvironment(const glm::vec3 cameraPos) const {
    if (WorldEnvironment::activeEnv != nullptr) {
        const EnvParameters& params = WorldEnvironment::activeEnv->params;

        GlobalEnvironmentData uboData;
        uboData.u_sunDirection = glm::vec4(params.sunDirection, 0.0f);
        uboData.u_sunColor = glm::vec4(params.sunColor, 0.0f);
        uboData.u_ambientColor = glm::vec4(params.ambientColor, 0.0f);
        uboData.u_fogColor = glm::vec4(params.fogColor, 0.0f);
        uboData.u_heightFogColor = glm::vec4(params.heightFogColor, 0.0f);
        uboData.u_causticsColor = glm::vec4(params.causticsColor, 0.0f);
        uboData.u_cameraPos = glm::vec4(cameraPos, 0.0f);

        uboData.u_sunEnergy = params.sunEnergy;
        uboData.u_ambientEnergy = params.ambientEnergy;
        uboData.u_baseFogDensity = params.fogDensity;
        uboData.u_heightFogMin = params.heightFogMin;

        uboData.u_heightFogMax = params.heightFogMax;
        uboData.u_causticsScale = params.causticsScale;
        uboData.u_causticsIntensity = params.causticsIntensity;
        uboData.u_depthDimmingCoefficient = params.depthDimmingCoefficient;

        uboData.u_time = static_cast<float>(glfwGetTime());
        uboData.u_bloomThreshold = params.bloomThreshold;
        uboData.u_bloomIntensity = params.bloomIntensity;

        uboData.u_fogEnabled = params.fogEnabled ? 1 : 0;
        uboData.u_heightFogEnabled = params.heightFogEnabled ? 1 : 0;
        uboData.u_causticsEnabled = params.causticsEnabled ? 1 : 0;
        uboData.u_depthDimmingEnabled = params.depthDimmingEnabled ? 1 : 0;
        uboData.u_bloomEnabled = params.bloomEnabled ? 1 : 0;

        // Upload the entire block to the GPU in a single step
        glBindBuffer(GL_UNIFORM_BUFFER, getEnvUBO());
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GlobalEnvironmentData), &uboData);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void Renderer::drawDebugAABBs(const glm::mat4& view, const glm::mat4& projection) const {
    if (!showDebugAABBs || debugAABBs.empty()) return;

    static GLuint debugShader = 0;
    if (debugShader == 0) {
        debugShader = ShaderManager::getInstance().loadShader(
            "debug",
            "assets/shaders/debug.vert",
            "assets/shaders/debug.frag"
        );
    }

    glUseProgram(debugShader);

    glm::mat4 vp = projection * view;
    Kern::setUniform(debugShader, "transformation", vp);

    // Draw wireframes in bright green
    Kern::setUniform(debugShader, "u_color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    for (const auto& box : debugAABBs) {
        drawDebugBox(box.first, box.second);
    }

    glUseProgram(0);
}

void Renderer::drawDebugOBB(
    const glm::vec3& localMin,
    const glm::vec3& localMax,
    const glm::mat4& modelMatrix,
    const glm::mat4& view,
    const glm::mat4& projection
) const {
    // 1. Generate 8 local-space corners (they never stretch or change scale in model-space)
    glm::vec3 vertices[8] = {
        glm::vec3(localMin.x, localMin.y, localMin.z),
        glm::vec3(localMin.x, localMin.y, localMax.z),
        glm::vec3(localMin.x, localMax.y, localMin.z),
        glm::vec3(localMin.x, localMax.y, localMax.z),
        glm::vec3(localMax.x, localMin.y, localMin.z),
        glm::vec3(localMax.x, localMin.y, localMax.z),
        glm::vec3(localMax.x, localMax.y, localMin.z),
        glm::vec3(localMax.x, localMax.y, localMax.z)
    };

    // 12 lines (24 indices) connecting the corners
    unsigned int indices[24] = {
        0, 1,  1, 3,  3, 2,  2, 0, // Bottom outline
        4, 5,  5, 7,  7, 6,  6, 4, // Top outline
        0, 4,  1, 5,  2, 6,  3, 7  // Vertical pillars
    };

    static GLuint obbVAO = 0;
    static GLuint obbVBO = 0;
    static GLuint obbEBO = 0;

    // Generate static buffers once on first call
    if (obbVAO == 0) {
        glGenVertexArrays(1, &obbVAO);
        glGenBuffers(1, &obbVBO);
        glGenBuffers(1, &obbEBO);
    }

    glBindVertexArray(obbVAO);

    // Upload vertices and indices to dynamic GPU buffers on the fly
    glBindBuffer(GL_ARRAY_BUFFER, obbVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obbEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Get the debug shader
    static GLuint debugShader = 0;
    if (debugShader == 0) {
        debugShader = ShaderManager::getInstance().loadShader(
            "debug",
            "assets/shaders/debug.vert",
            "assets/shaders/debug.frag"
        );
    }

    glUseProgram(debugShader);

    // Calculate Model-View-Projection matrix on the GPU
    glm::mat4 mvp = projection * view * modelMatrix;
    Kern::setUniform(debugShader, "transformation", mvp);
    Kern::setUniform(debugShader, "u_color", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)); // Draw OBB in bright orange

    // Draw using GL_LINES
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::drawElement(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) const {
    if (e.hasCustomRender()) {
        e.customRender(view, projection);
        return;
    }

    const Material& m = e.material;
    glUseProgram(m.shader);

    const glm::mat4 model = e.getGlobalModelMatrix();
    const glm::mat4 mvp = projection * view * model;
    Kern::setUniform(m.shader, "transformation", mvp);
    Kern::setUniform(m.shader, "modelMatrix",    model);

    e.prepareUniforms();

    if (const GLint invModelLoc = Kern::getUniformLocation(m.shader, "inverseModelMatrix"); invModelLoc != -1) {
        glm::mat4 invModel = glm::inverse(model);
        glUniformMatrix4fv(invModelLoc, 1, GL_FALSE, glm::value_ptr(invModel));
    }

    setupUniforms(m, cameraPos);
    Kern::DrawContext(e.mesh);
    glUseProgram(0);
}

void Renderer::setupUniforms(const Material& m, const glm::vec3& cameraPos) const {
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
        glBindTexture(GL_TEXTURE_CUBE_MAP, HimmelsboxWesen::aktiveCubemap);
    }

    Kern::setUniform(m.shader, "u_bloomStrength", m.bloomStrength);
}

void Renderer::drawDebugBox(const glm::vec3& min, const glm::vec3& max) const {
    // Generate the 8 world-space coordinates of the box
    glm::vec3 vertices[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(max.x, max.y, max.z)
    };

    // 12 lines (24 indices) connecting the corners
    unsigned int indices[24] = {
        0, 1,  1, 3,  3, 2,  2, 0, // Bottom face outline
        4, 5,  5, 7,  7, 6,  6, 4, // Top face outline
        0, 4,  1, 5,  2, 6,  3, 7  // Vertical pillars
    };

    static GLuint debugVAO = 0;
    static GLuint debugVBO = 0;
    static GLuint debugEBO = 0;

    if (debugVAO == 0) {
        glGenVertexArrays(1, &debugVAO);
        glGenBuffers(1, &debugVBO);
        glGenBuffers(1, &debugEBO);
    }

    glBindVertexArray(debugVAO);

    // Upload vertices and indices to dynamic GPU buffers on the fly
    glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Draw using GL_LINES
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Renderer::bindShaderToUBO(GLuint shaderProgram) {
    GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, "GlobalEnvironment");
    if (blockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(shaderProgram, blockIndex, 0);
    }
}
