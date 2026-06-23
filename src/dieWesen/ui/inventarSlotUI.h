#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTARSLOTUI_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTARSLOTUI_H

#include "werkzeuge/ui/wesenUI.h"
#include "werkzeuge/ui/uiLabel.h"
#include "werkzeuge/gegenstandDaten.h"

class InventarSlotUI : public UIElement {
public:
    void onInit() override;
    void setInhalt(GegenstandID id, int anzahl);
    void leeren();
    void setAusgewaehlt(bool ausgewaehlt);

private:
    UIElement* hintergrund = nullptr;
    UIElement* icon = nullptr;
    UILabel* anzahlLabel = nullptr;
    bool istAusgewaehlt = false;

    static GLuint slotTextur;
    static GLuint slotAusgewaehltTextur;
    static bool texturenGeladen;
    static void ladeTexturen();
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTARSLOTUI_H
