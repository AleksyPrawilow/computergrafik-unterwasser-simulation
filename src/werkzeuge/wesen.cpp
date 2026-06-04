//
// Created by s498780 on 01.06.2026.
//

#include "wesen.h"
#include <filesystem>
#include "glew.h"
#include "freeglut.h"
#include "textur.h"
#include "renderWerkzeuge.h"

void Wesen::loadTexture(const char *filepath) {
    material.albedo = Kern::LoadTexture(filepath);
}

void Wesen::loadNormalMap(const char *filepath) {
    material.normal = Kern::LoadTexture(filepath);
}

void Wesen::loadRoughnessMap(const char *filepath) {
    material.roughness = Kern::LoadTexture(filepath);
}

void Wesen::loadMetallicMap(const char *filepath) {
    material.metallic = Kern::LoadTexture(filepath);
}

void Wesen::loadModel(const char *filepath) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    mesh.initFromAssimpMesh(scene->mMeshes[0]);
}

void Wesen::loadShader(const char *filepath) {
}

void Wesen::init() {}

void Wesen::update(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
}
