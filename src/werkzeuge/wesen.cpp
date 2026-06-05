//
// Created by s498780 on 01.06.2026.
//

#include "wesen.h"
#include "freeglut.h"
#include "textur.h"
#include "renderWerkzeuge.h"

Wesen::~Wesen() {
    for (const Wesen* child : children) {
        delete child;
    }
    children.clear();

    if (parent != nullptr) {
        auto& pc = parent->children;
        pc.erase(std::remove(pc.begin(), pc.end(), this), pc.end());
    }
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

void Wesen::addChild(Wesen* wesen) {
    children.push_back(wesen);
    wesen->parent = this;
    wesen->init();
}

void Wesen::queueDestroy() {
    isQueuedDestroyed = true;
}

glm::mat4 Wesen::getGlobalModelMatrix() const {
    if (parent != nullptr) {
        return parent->getGlobalModelMatrix() * transform.getModelMatrix();
    }
    return transform.getModelMatrix();
}

void Wesen::init() {}

void Wesen::update(GLFWwindow* window, const float deltaTime, Transform& cameraTransform) {
    onUpdate(window, deltaTime, cameraTransform);
    for (Wesen* child : children) {
        child->update(window, deltaTime, cameraTransform);
    }

    children.erase(std::remove_if(children.begin(), children.end(), [](Wesen* child) {
        if (child->isQueuedDestroyed) {
            delete child;
            return true;
        }
        return false;
    }), children.end());
}

void Wesen::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {}

void Wesen::postRender(
    Renderer * renderer,
    const glm::mat4& viewProj,
    const glm::vec3& cameraPos
) const {
    for (const Wesen * wesen : children) {
        renderer->render(*wesen, viewProj, cameraPos);
        wesen->postRender(renderer, viewProj, cameraPos);
    }
}
