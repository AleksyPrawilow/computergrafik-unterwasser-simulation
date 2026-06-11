//
// Created by Alexey Pravilov on 11/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TEXTUREMANAGER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TEXTUREMANAGER_H
#include <string>
#include <unordered_map>
#include "glew.h"


class TextureManager {
public:
    static TextureManager& getInstance() {
        static TextureManager instance;
        return instance;
    }

    GLuint loadTexture(const std::string& filepath, bool pixelArt = false);

    void cleanup();

private:
    TextureManager() = default;
    ~TextureManager() { cleanup(); }

    std::unordered_map<std::string, GLuint> textures {};
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_TEXTUREMANAGER_H
