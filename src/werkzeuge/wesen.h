//
// Created by Aleksy Prawilow on 01.06.2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H
#include "glew.h"
#include "renderer.h"
#include "renderWerkzeuge.h"
#include "transform.h"

struct Material {
    GLuint shader = 0;
    GLuint albedo = 0;
    GLuint normal = 0;
    GLuint roughness = 0;
    GLuint metallic = 0;
};

class Renderer;

class Wesen {
public:
    virtual ~Wesen();
    Transform transform;
    Kern::RenderContext mesh;
    Material material;
    Wesen * parent = nullptr;
    std::vector<Wesen *> children;
    bool isQueuedDestroyed = false;

    void loadModel( const char * filepath );
    void addChild( Wesen * wesen );
    void queueDestroy();
    [[nodiscard]] glm::mat4 getGlobalModelMatrix() const;

    virtual void init();
    void update(GLFWwindow* window, float deltaTime, Transform& cameraTransform);
    virtual void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform);
    virtual void prepareUniforms() const {}
    void postRender(Renderer * renderer, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) const;
    [[nodiscard]] virtual bool hasCustomRender() const { return false; }
    virtual void customRender(const glm::mat4& view, const glm::mat4& projection) const {}
private:
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H