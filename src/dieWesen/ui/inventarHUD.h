#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTARHUD_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTARHUD_H

#include "werkzeuge/ui/wesenUI.h"
#include "werkzeuge/ui/uiContainers.h"
#include "werkzeuge/ui/uiLabel.h"
#include "inventarSlotUI.h"

class InventarHUD : public UIElement {
public:
    void onInit() override;
    void onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) override;

    bool istOffen() const { return offen; }

private:
    bool offen = false;
    bool playerWarAktiv = true;
    int ausgewaehlterIndex = 0;

    UIElement* hintergrund = nullptr;
    UILabel* titelLabel = nullptr;
    UILabel* hinweisLabel = nullptr;
    VBoxUI* rasterContainer = nullptr;
    InventarSlotUI* slots[9] = {};

    void umschalten(GLFWwindow* window);
    void aktualisieren();
    void auswahlAktualisieren();
    void navigieren();
    void ausruesten();
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTARHUD_H
