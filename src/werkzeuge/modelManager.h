//
// Created by Alexey Pravilov on 21/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MODELMANAGER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MODELMANAGER_H

#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "werkzeuge/renderWerkzeuge.h"
#include "werkzeuge/wesen.h"

struct CachedModel {
    Kern::RenderContext mesh;
    AABB localAABB;
};

class ModelManager {
public:
    static ModelManager& getInstance() {
        static ModelManager instance;
        return instance;
    }

    CachedModel getModel(const std::string& filepath) {
        // If the model is already loaded, return the cached mesh and bounds immediately
        if (auto it = cache.find(filepath); it != cache.end()) {
            return it->second;
        }

        // Otherwise, perform the Assimp disk-load once
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return CachedModel();
        }

        CachedModel model;
        model.mesh.initFromAssimpMesh(scene->mMeshes[0]);

        // Calculate local AABB boundaries
        const aiMesh* assimpMesh = scene->mMeshes[0];
        glm::vec3 minBound(1e30f);
        glm::vec3 maxBound(-1e30f);

        for (unsigned int v = 0; v < assimpMesh->mNumVertices; v++) {
            aiVector3D vertex = assimpMesh->mVertices[v];
            minBound.x = std::min(minBound.x, vertex.x);
            minBound.y = std::min(minBound.y, vertex.y);
            minBound.z = std::min(minBound.z, vertex.z);

            maxBound.x = std::max(maxBound.x, vertex.x);
            maxBound.y = std::max(maxBound.y, vertex.y);
            maxBound.z = std::max(maxBound.z, vertex.z);
        }
        model.localAABB.min = minBound;
        model.localAABB.max = maxBound;

        // Store in cache
        cache[filepath] = model;
        return model;
    }

    void cleanup() {
        // Clean up GPU buffers for all cached models on shutdown
        for (auto& [path, model] : cache) {
            if (model.mesh.vertexArray != 0) glDeleteVertexArrays(1, &model.mesh.vertexArray);
            if (model.mesh.vertexBuffer != 0) glDeleteBuffers(1, &model.mesh.vertexBuffer);
            if (model.mesh.vertexIndexBuffer != 0) glDeleteBuffers(1, &model.mesh.vertexIndexBuffer);
        }
        cache.clear();
    }

private:
    ModelManager() = default;
    std::unordered_map<std::string, CachedModel> cache;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_MODELMANAGER_H
