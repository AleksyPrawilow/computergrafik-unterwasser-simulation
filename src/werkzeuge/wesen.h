//
// Created by Aleksy Prawilow on 01.06.2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H
#include "glew.h"
#include "renderWerkzeuge.h"
#include "transform.h"

struct Material {
    GLuint shader = 0;
    GLuint albedo = 0;
    GLuint normal = 0;
    GLuint roughness = 0;
    GLuint metallic = 0;
};

class Wesen {
public:
    virtual ~Wesen() = default;
    Transform transform;
    Kern::RenderContext mesh;
    Material material;

    void loadTexture( const char * filepath );
    void loadNormalMap( const char * filepath );
    void loadRoughnessMap( const char * filepath );
    void loadMetallicMap( const char * filepath );
    void loadModel( const char * filepath );
    void loadShader( const char * filepath );

    virtual void init();
    virtual void update(GLFWwindow* window, float deltaTime, Transform& cameraTransform);
    virtual void prepareUniforms() const {}
private:
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H