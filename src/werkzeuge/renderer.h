//
// Created by Alexey Pravilov on 01/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
#include "wesen.h"
#include <glm.hpp>
#include <vector>
#include <unordered_map>
#include "frustum.h"

class Wesen;
struct Material;

// Inside renderer.h (or a shared engine types file)
struct alignas(16) GlobalEnvironmentData {
    // 16-byte aligned members (7 vectors = 112 bytes)
    glm::vec4 u_sunDirection;
    glm::vec4 u_sunColor;
    glm::vec4 u_ambientColor;
    glm::vec4 u_fogColor;
    glm::vec4 u_heightFogColor;
    glm::vec4 u_causticsColor;
    glm::vec4 u_cameraPos;

    // 4-byte members grouped into 16-byte blocks (4 floats = 16 bytes)
    float u_sunEnergy;
    float u_ambientEnergy;
    float u_baseFogDensity;
    float u_heightFogMin;

    // Next 16-byte block (4 floats = 16 bytes)
    float u_heightFogMax;
    float u_causticsScale;
    float u_causticsIntensity;
    float u_depthDimmingCoefficient;

    // Next 16-byte block (3 floats + 1 int = 16 bytes)
    float u_time;
    float u_bloomThreshold;
    float u_bloomIntensity;
    int u_fogEnabled; // Booleans are represented as 32-bit ints in GLSL

    // Next 16-byte block (4 ints = 16 bytes)
    int u_heightFogEnabled;
    int u_causticsEnabled;
    int u_depthDimmingEnabled;
    int u_bloomEnabled;
}; // Total struct size = 112 + 16 + 16 + 16 + 16 = 176 bytes (Perfect 16-byte multiple)

class Renderer {
public:
    void init();
    void render(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void drawOpaque(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void drawHimmelsbox(const glm::mat4& view, const glm::mat4& projection) const;
    void drawTransparent(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void drawUI(const glm::mat4& view, const glm::mat4& projection);
    void clearQueues() {
        opaqueQueue.clear();
        himmelsboxQueue.clear();
        transparentQueue.clear();
        uiQueue.clear();
        himmelsboxQueue.clear();
        debugAABBs.clear();
    }

    void updateFrustum(const glm::mat4& view, const glm::mat4& projection);
    void sendEnvironment(glm::vec3 cameraPos) const;
    GLuint getEnvUBO() const { return envUBO; }
    static void bindShaderToUBO(GLuint shaderProgram);

    bool showDebugAABBs = false;
    void drawDebugAABBs(const glm::mat4& view, const glm::mat4& projection) const;

private:
    void drawElement(const Wesen& e, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) const;
    void setupUniforms(const Material& m, const glm::vec3& cameraPos) const;
    void drawDebugBox(const glm::vec3& min, const glm::vec3& max) const;

    std::vector<const Wesen*> opaqueQueue;
    std::vector<const Wesen*> himmelsboxQueue;
    std::vector<const Wesen*> transparentQueue;
    std::vector<const Wesen*> uiQueue;
    mutable std::vector<std::pair<glm::vec3, glm::vec3>> debugAABBs;

    GLuint defaultNormal = 0;
    GLuint defaultEmission = 0;
    GLuint defaultOpacity = 0;
    GLuint envUBO = 0;
    Frustum frustum{};
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RENDERER_H
