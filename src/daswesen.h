//
// Created by Aleksy Prawilow on 01.06.2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H
#include "glew.h"
#include "Render_Utils.h"


class daswesen {
public:
    daswesen();
    ~daswesen();
    void loadTexture( const char * filepath );
    void loadNormalMap( const char * filepath );
    void loadRoughnessMap( const char * filepath );
    void loadMetallicMap( const char * filepath );
    void loadModel( const char * filepath );
    void loadShader( const char * filepath );
    void render();
private:
    GLuint texture;
    GLuint normal_map;
    GLuint roughness_map;
    GLuint metallic_map;
    Core::RenderContext model;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_DASWESEN_H