//
// Created by Alexey Pravilov on 11/06/2026.
//

#include "textureManager.h"
#include "SOIL.h"
#include <iostream>

GLuint TextureManager::loadTexture(const std::string& filepath, const bool pixelArt) {
    if (const auto it = textures.find(filepath); it != textures.end()) {
        return it->second;
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    if (pixelArt) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int w, h;
    unsigned char* image = SOIL_load_image(filepath.c_str(), &w, &h, nullptr, SOIL_LOAD_RGBA);

    if (image == nullptr) {
        std::cerr << "Failed to load texture file: " << filepath << std::endl;
        glDeleteTextures(1, &id);
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    textures[filepath] = id;
    return id;
}

void TextureManager::cleanup() {
    for (auto& [fst, snd] : textures) {
        glDeleteTextures(1, &snd);
    }
    textures.clear();
}