#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OFENHUD_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OFENHUD_H

#include "handwerkHUD.h"

class OfenHUD : public UIElement {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    bool offen = false;
    bool playerWarAktiv = true;
    int ausgewaehlteZeile = 0;
    int scrollOffset = 0;
    static constexpr int MAX_SICHTBAR = 6;

    UIElement* hintergrund = nullptr;
    UIElement* container = nullptr;
    UILabel* titelLabel = nullptr;
    UILabel* hinweisLabel = nullptr;

    std::vector<Rezept> rezepte;
    std::vector<RezeptZeileUI*> zeilen;

    void umschalten(GLFWwindow* window);
    void herstellen();
    void aktualisieren();
    void scrollAktualisieren();
    bool spielerNaheOfen() const;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_OFENHUD_H
