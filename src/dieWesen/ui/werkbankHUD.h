#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WERKBANKHUD_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WERKBANKHUD_H

#include "handwerkHUD.h"

class WerkbankHUD : public UIElement {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

private:
    bool offen = false;
    int ausgewaehlteZeile = 0;
    int scrollOffset = 0;
    static constexpr int MAX_SICHTBAR = 6;

    UIElement* hintergrund = nullptr;
    VBoxUI* container = nullptr;
    UILabel* titelLabel = nullptr;
    UILabel* hinweisLabel = nullptr;

    std::vector<HandwerkRezept> rezepte;
    std::vector<RezeptZeileUI*> zeilen;

    void umschalten(GLFWwindow* window);
    void rezepteRegistrieren();
    void herstellen();
    void aktualisieren();
    void scrollAktualisieren();
    bool spielerNaheWerkbank() const;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WERKBANKHUD_H
