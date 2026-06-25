#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAUMSCHIFF_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_RAUMSCHIFF_H

#include "particleEmitter.h"
#include "timer.h"
#include "../werkzeuge/wesen.h"
#include "../werkzeuge/transform.h"
#include "ui/fadenkreuz.h"
#include "werkzeuge/audio/audioPlayer.h"
#include <functional>
#include <unordered_set>

enum class AnsichtModus {
    ERSTE_PERSON,
    DRITTE_PERSON
};

class Raumschiff : public Wesen {
public:
    void init() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;
    [[nodiscard]] float getGeschwindigkeit() const { return tatsaechlicheGeschwindigkeit; }
    [[nodiscard]] float getLeben() const { return leben; }
    void schadenNehmen(float schaden);
    void heilen(float menge = 100.0f) { leben = glm::min(leben + menge, 100.0f); }
    float schadenBlitz = 0.0f;
    void vollHeilen() { leben = 100.0f; }
    void raketenAuffuellen() { raketenMunition = 3; }
    [[nodiscard]] int getRaketenMunition() const { return raketenMunition; }
    [[nodiscard]] bool hatSpawnSchutz() const { return spawnSchutz > 0.0f; }
    void setSpawnSchutz(float dauer) { spawnSchutz = dauer; }
    std::function<void()> onDeath;

private:
    Fadenkreuz * fadenkreuz = nullptr;
    AnsichtModus ansichtModus = AnsichtModus::DRITTE_PERSON;
    ParticleEmitter * triebwerke[2] = {nullptr};
    Timer * canShootTimer = nullptr;
    AudioPlayer * shootSound = nullptr;

    float tatsaechlicheGeschwindigkeit = 0.0f;
    float winkelGeschwindigkeit = 4.0f;
    float bewegungsGeschwindigkeit = 60.0f;
    float rueckwaertsGeschwindigkeit = 7.0f;
    float zielGeschwindigkeit = 0.0f;
    float zielRollGeschwindigkeit = 0.0f;
    float maxDashSpeed = 100.0f;
    float currentDashSpeed = 0.0f;
    bool isDashing = false;
    glm::vec3 dashDir = glm::vec3(0.0f);
    float dashRollAngle = 0.0f;
    float lastDashRollAngle = 0.0f;
    float dashCameraOvertakeFactor = 0.0f;
    float leben = 100.0f;
    bool istAktiv = true;
    bool canShoot = true;
    bool shootLeft = true;
    int raketenMunition = 3;
    float spawnSchutz = 0.0f;
    std::unordered_set<Wesen*> aktiveKollisionen;

    double letzteX = 0.0;
    double letzteY = 0.0;
    bool ersteMaus = true;

    float nickGeschwindigkeit = 0.0f;
    float rollGeschwindigkeit = 0.0f;
    float gierGeschwindigkeit = 0.0f;

    void eingabeVerarbeiten(GLFWwindow* window, float deltaTime);
    void kameraAktualisieren(Transform& cameraTransform, float deltaTime) const;
    void rollsBehandeln(GLFWwindow* window, float deltaTime);
};

#endif
