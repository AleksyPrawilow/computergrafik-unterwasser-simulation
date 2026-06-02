//
// Created by s498780 on 01.06.2026.
//

#include "daswesen.h"
#include <filesystem>
#include "glew.h"
#include "freeglut.h"
#include "textur.h"
#include "renderWerkzeuge.h"

void Daswesen::loadTexture(const char *filepath) {
    material.albedo = Core::LoadTexture(filepath);
}

void Daswesen::loadNormalMap(const char *filepath) {
    material.normal = Core::LoadTexture(filepath);
}

void Daswesen::loadRoughnessMap(const char *filepath) {
    material.roughness = Core::LoadTexture(filepath);
}

void Daswesen::loadMetallicMap(const char *filepath) {
    material.metallic = Core::LoadTexture(filepath);
}

void Daswesen::loadModel(const char *filepath) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    mesh.initFromAssimpMesh(scene->mMeshes[0]);
}

void Daswesen::loadShader(const char *filepath) {
}

void Daswesen::init() {}

void Daswesen::update(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
}
