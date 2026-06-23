#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GEGENSTANDDATEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GEGENSTANDDATEN_H

#include <string>
#include <unordered_map>
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
    TRUHE
};

struct GegenstandInfo {
    GegenstandID id = GegenstandID::KEINE;
    std::string name;
    std::string iconPfad;
    GLuint iconTextur = 0;
    int maxStapel = 64;
};

class GegenstandDaten {
public:
    static GegenstandDaten& getInstance() {
        static GegenstandDaten instance;
        return instance;
    }

    void init();
    const GegenstandInfo& getInfo(GegenstandID id) const;

private:
    GegenstandDaten() = default;
    void registrieren(GegenstandID id, const std::string& name,
                      const std::string& iconPfad, int maxStapel = 64);
    std::unordered_map<int, GegenstandInfo> registry;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GEGENSTANDDATEN_H
