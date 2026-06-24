#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GEGENSTANDDATEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GEGENSTANDDATEN_H

#include <string>
#include <unordered_map>
#include <vector>
#include "glew.h"

enum class GegenstandID {
    KEINE = 0,
    HOLZ,
    STEIN,
    PLANKE,
    HOLZAXT,
    AXT,
    FLASCHE,
    SEIL,
    FLOSS,
    WERKBANK,
    SCHAUFEL,
    ERDE,
    FACKEL,
    ZAUN,
    TRUHE,
    KARTE,
    MINIUBOOT,
    ANGEL,
    FISCH,
    SEETANG,
    SUSHI
};

enum class WerkzeugTyp {
    KEINS,
    AXT,
    SCHAUFEL,
    ANGEL
};

enum class RezeptQuelle {
    HAND,
    WERKBANK
};

struct GegenstandInfo {
    GegenstandID id = GegenstandID::KEINE;
    std::string name;
    std::string iconPfad;
    GLuint iconTextur = 0;
    int maxStapel = 64;

    WerkzeugTyp werkzeugTyp = WerkzeugTyp::KEINS;
    int werkzeugSchaden = 0;
    bool istPlatzierbar = false;
    std::string modellPfad;
    std::string modellAlbedoPfad;
};

struct Rezept {
    GegenstandID eingabe1;
    GegenstandID eingabe2;
    GegenstandID ausgabe;
    int ausgabeAnzahl = 1;
    RezeptQuelle quelle = RezeptQuelle::HAND;
};

class GegenstandDaten {
public:
    static GegenstandDaten& getInstance() {
        static GegenstandDaten instance;
        return instance;
    }

    void init();
    const GegenstandInfo& getInfo(GegenstandID id) const;
    const std::vector<Rezept>& getRezepte() const { return rezepte; }
    std::vector<Rezept> getRezepteFuer(RezeptQuelle quelle) const;

private:
    GegenstandDaten() = default;
    void registrieren(GegenstandID id, const std::string& name,
                      const std::string& iconPfad, int maxStapel = 64);
    void rezeptHinzufuegen(GegenstandID e1, GegenstandID e2,
                           GegenstandID aus, int anzahl = 1,
                           RezeptQuelle quelle = RezeptQuelle::HAND);
    std::unordered_map<int, GegenstandInfo> registry;
    std::vector<Rezept> rezepte;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GEGENSTANDDATEN_H
