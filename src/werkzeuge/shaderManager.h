//
// Created by Alexey Pravilov on 02/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SHADERMANAGER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SHADERMANAGER_H
#include <string>
#include <unordered_map>

#include "glew.h"
#include "shaderLoader.h"


class ShaderManager {
private:
    ShaderManager() {}
    Kern::shaderLoader shaderLoader{};
    std::unordered_map<std::string, GLuint> shaderMap{};
public:
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    static ShaderManager& getInstance() {
        static ShaderManager instance;
        return instance;
    }

    GLuint getShader(const std::string &name);
    GLuint loadShader(const std::string& name, const char* vertPath, const char* fragPath);
    void cleanup();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_SHADERMANAGER_H
