//
// Created by Alexey Pravilov on 17/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AXE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AXE_H
#include "tree.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/audio/audioPlayer.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/modelManager.h"


class Timer;

class Axe: public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
private:
    bool isPlayingAnimation = false;
    bool recoveringAnimation = false;
    bool istAxt = false;
    glm::vec3 weaponEuler;
    glm::vec3 hitNormal;
    Tree * treeToHit = nullptr;
    Timer * recoveryTimer = nullptr;
    AudioPlayer * hitSound = nullptr;
    AudioPlayer * swingSound = nullptr;

    GLuint defaultAlbedo = 0;
    GLuint upgradedAlbedo = 0;
    GegenstandID letzteAktivesItem = GegenstandID::KEINE;

    Kern::RenderContext axeModelMesh;
    AABB axeModelAABB;
    Kern::RenderContext cubeModelMesh;
    AABB cubeModelAABB;

    void swing();
    void recoverAnimation();
    void hitTree();
    void ausruestungAktualisieren();
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_AXE_H
