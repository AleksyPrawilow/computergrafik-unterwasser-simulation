//
// Created by s498780 on 01.06.2026.
//

#include "wesen.h"
#include "freeglut.h"
#include "groupManager.h"
#include "textur.h"
#include "renderWerkzeuge.h"
#include "visual/tween.h"

Wesen::~Wesen() {
    for (Wesen* child : children) {
        child->parent = nullptr;
        delete child;
    }
    children.clear();

    TweenManager::getInstance().killTweensOwnedBy(this);

    for (const std::string& groupName : myGroups) {
        GroupManager::getInstance().removeFromGroup(groupName, this);
    }
    myGroups.clear();

    if (parent != nullptr) {
        auto& pc = parent->children;
        pc.erase(std::remove(pc.begin(), pc.end(), this), pc.end());
    }
}

void Wesen::loadModel(const char *filepath, std::vector<glm::vec3> * vertices) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    if (vertices != nullptr) {
        vertices->clear();
        for (unsigned int i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
            vertices->emplace_back(scene->mMeshes[0]->mVertices[i].x, scene->mMeshes[0]->mVertices[i].y, scene->mMeshes[0]->mVertices[i].z);
        }
    }
    mesh.initFromAssimpMesh(scene->mMeshes[0]);
}

void Wesen::addChild(Wesen* wesen) {
    children.push_back(wesen);
    wesen->parent = this;
    wesen->init();
}

float Wesen::getUIScaleFactor() const {
    return (parent != nullptr) ? parent->getUIScaleFactor() : 1.0f;
}

void Wesen::queueDestroy() {
    isQueuedDestroyed = true;
}

Tween* Wesen::createTween() {
    return TweenManager::getInstance().createTween()->setOwner(this);
}

void Wesen::addToGroup(const std::string& groupName) {
    if (std::find(myGroups.begin(), myGroups.end(), groupName) == myGroups.end()) {
        myGroups.push_back(groupName);
        GroupManager::getInstance().addToGroup(groupName, this);
    }
}

void Wesen::removeFromGroup(const std::string& groupName) {
    if (const auto it = std::find(myGroups.begin(), myGroups.end(), groupName); it != myGroups.end()) {
        myGroups.erase(it);
        GroupManager::getInstance().removeFromGroup(groupName, this);
    }
}

bool Wesen::isInGroup(const std::string& groupName) const {
    return std::find(myGroups.begin(), myGroups.end(), groupName) != myGroups.end();
}

const std::vector<Wesen*>& Wesen::getNodesInGroup(const std::string& groupName) {
    return GroupManager::getInstance().getEntitiesInGroup(groupName);
}

void Wesen::init() {}

void Wesen::update(GLFWwindow* window, const float deltaTime, Transform& cameraTransform) {
    onUpdate(window, deltaTime, cameraTransform);

    updateGlobalTransforms();

    auto kinderKopie = children;
    for (Wesen* child : kinderKopie) {
        child->update(window, deltaTime, cameraTransform);
    }

    children.erase(std::remove_if(children.begin(), children.end(), [](Wesen* child) {
        if (child->isQueuedDestroyed) {
            child->parent = nullptr;

            delete child;
            return true;
        }
        return false;
    }), children.end());
}

void Wesen::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {}

void Wesen::postRender(
    Renderer * renderer,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos
) const {
    for (const Wesen * wesen : children) {
        renderer->render(*wesen, view, projection, cameraPos);
        wesen->postRender(renderer, view, projection, cameraPos);
    }
}

void Wesen::updateGlobalTransforms() {
    if (parent == nullptr) {
        cachedGlobalModelMatrix = transform.getModelMatrix();
        cachedGlobalTransform = transform;
        return;
    }

    auto [position, rotation, scale] = parent->getGlobalTransform();

    cachedGlobalTransform.scale = scale * transform.scale;
    cachedGlobalTransform.rotation = rotation * transform.rotation;
    cachedGlobalTransform.position = position + (rotation * (transform.position * scale));

    cachedGlobalModelMatrix = parent->getGlobalModelMatrix() * transform.getModelMatrix();
}
