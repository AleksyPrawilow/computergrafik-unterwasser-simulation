#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HANDWERKHUD_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HANDWERKHUD_H

#include "werkzeuge/ui/uiContainers.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/ui/wesenUI.h"
#include "werkzeuge/gegenstandDaten.h"

class RezeptZeileUI;

class HandwerkHUD : public UIElement {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    bool offen = false;
    bool playerWarAktiv = true;
    int ausgewaehlteZeile = 0;
    int scrollOffset = 0;
    static constexpr int MAX_SICHTBAR = 8;

    void scrollAktualisieren();

    UIElement* hintergrund = nullptr;
    UIElement* container = nullptr;
    UILabel* titelLabel = nullptr;
    UILabel* hinweisLabel = nullptr;

    std::vector<Rezept> rezepte;
    std::vector<RezeptZeileUI*> zeilen;

    void umschalten(GLFWwindow* window);
    void herstellen();
    void aktualisieren();
};

class RezeptZeileUI : public HBoxUI {
public:
    Rezept rezept;
    UILabel* cursorLabel = nullptr;
    UIElement* icon1 = nullptr;
    UILabel* plusLabel = nullptr;
    UIElement* icon2 = nullptr;
    UILabel* pfeilLabel = nullptr;
    UIElement* ergebnisIcon = nullptr;
    UILabel* nameLabel = nullptr;
    bool herstellbar = false;
    bool istHervorgehoben = false;

    void onInit() override;
    void aktualisieren();
    void setHervorgehoben(bool hervorgehoben);
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HANDWERKHUD_H
