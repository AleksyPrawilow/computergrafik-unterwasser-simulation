//
// Created by Alexey Pravilov on 17/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HELD_ITEM_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HELD_ITEM_H
#include "island.h"
#include "raft.h"
#include "tree.h"
#include "uboot.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/audio/audioPlayer.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/modelManager.h"


class Timer;

class HeldItem: public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    void prepareUniforms() const override;
private:
    bool isPlayingAnimation = false;
    bool recoveringAnimation = false;
    bool istAxt = false;
    glm::vec3 weaponEuler;
    glm::vec3 hitNormal;
    Tree * treeToHit = nullptr;
    Timer * recoveryTimer = nullptr;
    AudioPlayer * hitSound = nullptr;
    AudioPlayer * shovelHitSound = nullptr;
    AudioPlayer * swingSound = nullptr;
    Island * island = nullptr;

    GLuint defaultAlbedo = 0;
    GLuint upgradedAlbedo = 0;
    GLuint axeNormal = 0;
    GLuint axeRoughness = 0;
    GLuint axeMetallic = 0;

    GLuint shovelAlbedo = 0;
    GLuint shovelNormal = 0;
    GLuint shovelMetallic = 0;
    GegenstandID letzteAktivesItem = GegenstandID::KEINE;

    Kern::RenderContext axeModelMesh;
    AABB axeModelAABB;
    Kern::RenderContext cubeModelMesh;
    AABB cubeModelAABB;
    Kern::RenderContext mapModelMesh;
    AABB mapModelAABB;
    Kern::RenderContext shovelModelMesh;
    AABB shovelModelAABB;
    GLuint mapAlbedo = 0;
    GLuint mapShader = 0;

    bool karteOffen = false;
    bool karteAnimiert = false;
    glm::vec3 karteIdlePos = glm::vec3(0.3f, -0.35f, -0.7f);
    glm::vec3 karteLesePos = glm::vec3(0.0f, -0.15f, -0.75f);
    glm::vec3 karteIdleEuler = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 karteLeseEuler = glm::vec3(-82.0f, 0.0f, 0.0f);

    void swing();
    void recoverAnimation();
    void hitTree();
    void hitGround() const;
    void shovelDig();
    void shovelRecover();
    void throwSub();
    void showcaseSub(Uboot * uboot);
    void throwRaft();
    void ausruestungAktualisieren();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HELD_ITEM_H
