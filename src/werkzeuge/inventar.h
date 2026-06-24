#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTAR_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTAR_H

#include "gegenstandDaten.h"
#include <array>
#include <functional>
#include <vector>

struct InventarPlatz {
    GegenstandID id = GegenstandID::KEINE;
    int anzahl = 0;

    bool istLeer() const { return id == GegenstandID::KEINE || anzahl <= 0; }
};

class Inventar {
public:
    static Inventar& getInstance() {
        static Inventar instance;
        return instance;
    }

    static constexpr int ZEILEN = 3;
    static constexpr int SPALTEN = 3;
    static constexpr int GROESSE = ZEILEN * SPALTEN;
    static constexpr int HOTBAR_GROESSE = 5;

    int hinzufuegen(GegenstandID id, int anzahl = 1);
    bool entfernen(GegenstandID id, int anzahl = 1);
    int zaehlen(GegenstandID id) const;
    bool passtRein(GegenstandID id, int anzahl = 1) const;

    InventarPlatz& getPlatz(int index);
    const InventarPlatz& getPlatz(int index) const;
    void tauschen(int indexA, int indexB);

    InventarPlatz& getHotbar(int index);
    const InventarPlatz& getHotbar(int index) const;
    void ausruesten(int hotbarIndex, GegenstandID id);
    void hotbarEntfernen(int hotbarIndex);
    int getAktiverSlot() const { return aktiverSlot; }
    void setAktiverSlot(int index);
    GegenstandID getAktivesItem() const;
    void benachrichtigen();

    void addOnChanged(std::function<void()> callback);
    void reset();

private:
    Inventar() = default;
    std::array<InventarPlatz, GROESSE> plaetze{};
    std::array<InventarPlatz, HOTBAR_GROESSE> hotbar{};
    int aktiverSlot = 0;
    std::vector<std::function<void()>> onChangedCallbacks;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_INVENTAR_H
