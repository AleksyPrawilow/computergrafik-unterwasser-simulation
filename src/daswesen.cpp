//
// Created by s498780 on 01.06.2026.
//

#include "daswesen.h"

#include <filesystem>

#include "glew.h"
#include "freeglut.h"
#include "texture.h"
#include "Render_Utils.h"

void daswesen::loadTexture(const char *filepath) {
    texture = Core::LoadTexture(filepath);
}

void daswesen::loadNormalMap(const char *filepath) {
    normal_map = Core::LoadTexture(filepath);
}

void daswesen::loadRoughnessMap(const char *filepath) {
    roughness_map = Core::LoadTexture(filepath);
}

void daswesen::loadMetallicMap(const char *filepath) {
    metallic_map = Core::LoadTexture(filepath);
}

void daswesen::loadModel(const char *filepath) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    model.initFromAssimpMesh(scene->mMeshes[0]);
}

void daswesen::loadShader(const char *filepath) {
}


void daswesen::render() {
    // void drawObjectShip(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint textureID2, float a, float b) {
    //     GLuint prog = programShip;
    //     glUseProgram(prog);
    //     glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
    //     glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
    //     glUniformMatrix4fv(glGetUniformLocation(prog, "transformation"), 1, GL_FALSE, (float*)&transformation);
    //     glUniformMatrix4fv(glGetUniformLocation(prog, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
    //     glUniform3f(glGetUniformLocation(prog, "lightPos"), -5, 3, 3);
    //     glUniform3f(glGetUniformLocation(prog, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    //
    //     Core::SetActiveTexture(textureID, "colorTexture", prog, 0);
    //     Core::SetActiveTexture(textureID2, "normalMap", prog, 1);
    //
    //     Core::DrawContext(context);
    //     glUseProgram(0);
    // }
}
