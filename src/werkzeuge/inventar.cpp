#include "inventar.h"

int Inventar::hinzufuegen(GegenstandID id, int anzahl) {
    if (id == GegenstandID::KEINE || anzahl <= 0) return anzahl;

    const int maxStapel = GegenstandDaten::getInstance().getInfo(id).maxStapel;

    for (auto& platz : plaetze) {
        if (anzahl <= 0) break;
        if (platz.id == id && platz.anzahl < maxStapel) {
            int frei = maxStapel - platz.anzahl;
            int hinzu = std::min(anzahl, frei);
            platz.anzahl += hinzu;
            anzahl -= hinzu;
        }
    }

    for (auto& platz : plaetze) {
        if (anzahl <= 0) break;
        if (platz.istLeer()) {
            platz.id = id;
            int hinzu = std::min(anzahl, maxStapel);
            platz.anzahl = hinzu;
            anzahl -= hinzu;
        }
    }

    benachrichtigen();
    return anzahl;
}

bool Inventar::entfernen(GegenstandID id, int anzahl) {
    if (id == GegenstandID::KEINE || anzahl <= 0) return false;
    if (zaehlen(id) < anzahl) return false;

    for (auto& platz : plaetze) {
        if (anzahl <= 0) break;
        if (platz.id == id) {
            int weg = std::min(anzahl, platz.anzahl);
            platz.anzahl -= weg;
            anzahl -= weg;
            if (platz.anzahl <= 0) {
                platz.id = GegenstandID::KEINE;
                platz.anzahl = 0;
            }
        }
    }

    benachrichtigen();
    return true;
}

int Inventar::zaehlen(GegenstandID id) const {
    int summe = 0;
    for (const auto& platz : plaetze) {
        if (platz.id == id) summe += platz.anzahl;
    }
    return summe;
}

bool Inventar::passtRein(GegenstandID id, int anzahl) const {
    if (id == GegenstandID::KEINE) return false;
    const int maxStapel = GegenstandDaten::getInstance().getInfo(id).maxStapel;
    int rest = anzahl;

    for (const auto& platz : plaetze) {
        if (rest <= 0) return true;
        if (platz.id == id && platz.anzahl < maxStapel) {
            rest -= (maxStapel - platz.anzahl);
        } else if (platz.istLeer()) {
            rest -= maxStapel;
        }
    }
    return rest <= 0;
}

InventarPlatz& Inventar::getPlatz(int index) {
    return plaetze[index];
}

const InventarPlatz& Inventar::getPlatz(int index) const {
    return plaetze[index];
}

void Inventar::tauschen(int indexA, int indexB) {
    std::swap(plaetze[indexA], plaetze[indexB]);
    benachrichtigen();
}

InventarPlatz& Inventar::getHotbar(int index) {
    return hotbar[index];
}

const InventarPlatz& Inventar::getHotbar(int index) const {
    return hotbar[index];
}

void Inventar::ausruesten(int hotbarIndex, GegenstandID id) {
    if (hotbarIndex < 0 || hotbarIndex >= HOTBAR_GROESSE) return;
    if (id == GegenstandID::KEINE) return;

    if (!entfernen(id, 1)) return;

    if (!hotbar[hotbarIndex].istLeer()) {
        hinzufuegen(hotbar[hotbarIndex].id, 1);
    }

    hotbar[hotbarIndex].id = id;
    hotbar[hotbarIndex].anzahl = 1;
    benachrichtigen();
}

void Inventar::hotbarEntfernen(int hotbarIndex) {
    if (hotbarIndex < 0 || hotbarIndex >= HOTBAR_GROESSE) return;
    if (hotbar[hotbarIndex].istLeer()) return;

    hinzufuegen(hotbar[hotbarIndex].id, hotbar[hotbarIndex].anzahl);
    hotbar[hotbarIndex].id = GegenstandID::KEINE;
    hotbar[hotbarIndex].anzahl = 0;
    benachrichtigen();
}

void Inventar::setAktiverSlot(int index) {
    if (index >= 0 && index < HOTBAR_GROESSE) {
        aktiverSlot = index;
        benachrichtigen();
    }
}

GegenstandID Inventar::getAktivesItem() const {
    return hotbar[aktiverSlot].id;
}

void Inventar::addOnChanged(std::function<void()> callback) {
    onChangedCallbacks.push_back(std::move(callback));
}

void Inventar::reset() {
    for (auto& platz : plaetze) {
        platz.id = GegenstandID::KEINE;
        platz.anzahl = 0;
    }
    for (auto& platz : hotbar) {
        platz.id = GegenstandID::KEINE;
        platz.anzahl = 0;
    }
    aktiverSlot = 0;
    onChangedCallbacks.clear();
}

void Inventar::benachrichtigen() {
    for (auto& cb : onChangedCallbacks) {
        if (cb) cb();
    }
}
