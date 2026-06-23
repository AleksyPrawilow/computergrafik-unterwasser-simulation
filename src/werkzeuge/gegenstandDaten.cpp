#include "gegenstandDaten.h"
#include "textur.h"

void GegenstandDaten::init() {
    // --- Items ---
    registrieren(GegenstandID::HOLZ, "Wood", "assets/textures/icon_holz.png", 64);
    registrieren(GegenstandID::STEIN, "Stone", "assets/textures/icon_stein.png", 64);
    registrieren(GegenstandID::PLANKE, "Plank", "assets/textures/icon_planke.png", 64);
    registrieren(GegenstandID::SEIL, "Rope", "assets/textures/icon_seil.png", 16);
    registrieren(GegenstandID::FLASCHE, "Bottle", "assets/textures/icon_flasche.png", 16);
    registrieren(GegenstandID::ERDE, "Dirt", "assets/textures/icon_erde.png", 64);

    // --- Tools ---
    auto& holzaxt = registry[static_cast<int>(GegenstandID::HOLZAXT)];
    registrieren(GegenstandID::HOLZAXT, "Wooden Axe", "assets/textures/icon_holzaxt.png", 1);
    registry[static_cast<int>(GegenstandID::HOLZAXT)].werkzeugTyp = WerkzeugTyp::AXT;
    registry[static_cast<int>(GegenstandID::HOLZAXT)].werkzeugSchaden = 1;

    registrieren(GegenstandID::AXT, "Axe", "assets/textures/icon_axt.png", 1);
    registry[static_cast<int>(GegenstandID::AXT)].werkzeugTyp = WerkzeugTyp::AXT;
    registry[static_cast<int>(GegenstandID::AXT)].werkzeugSchaden = 5;

    registrieren(GegenstandID::SCHAUFEL, "Shovel", "assets/textures/icon_schaufel.png", 1);
    registry[static_cast<int>(GegenstandID::SCHAUFEL)].werkzeugTyp = WerkzeugTyp::SCHAUFEL;

    // --- Placeables ---
    registrieren(GegenstandID::WERKBANK, "Workbench", "assets/textures/icon_werkbank.png", 1);
    registry[static_cast<int>(GegenstandID::WERKBANK)].istPlatzierbar = true;

    registrieren(GegenstandID::FLOSS, "Raft", "assets/textures/icon_floss.png", 1);

    registrieren(GegenstandID::FACKEL, "Torch", "assets/textures/icon_fackel.png", 16);
    registry[static_cast<int>(GegenstandID::FACKEL)].istPlatzierbar = true;

    registrieren(GegenstandID::ZAUN, "Fence", "assets/textures/icon_zaun.png", 16);
    registry[static_cast<int>(GegenstandID::ZAUN)].istPlatzierbar = true;

    registrieren(GegenstandID::TRUHE, "Chest", "assets/textures/icon_truhe.png", 1);
    registry[static_cast<int>(GegenstandID::TRUHE)].istPlatzierbar = true;

    // --- Hand recipes ---
    rezeptHinzufuegen(GegenstandID::HOLZ, GegenstandID::HOLZ, GegenstandID::PLANKE, 1, RezeptQuelle::HAND);
    rezeptHinzufuegen(GegenstandID::HOLZ, GegenstandID::STEIN, GegenstandID::AXT, 1, RezeptQuelle::HAND);
    rezeptHinzufuegen(GegenstandID::PLANKE, GegenstandID::SEIL, GegenstandID::FLOSS, 1, RezeptQuelle::HAND);
    rezeptHinzufuegen(GegenstandID::HOLZ, GegenstandID::PLANKE, GegenstandID::WERKBANK, 1, RezeptQuelle::HAND);
    rezeptHinzufuegen(GegenstandID::PLANKE, GegenstandID::STEIN, GegenstandID::SCHAUFEL, 1, RezeptQuelle::HAND);

    // --- Workbench recipes ---
    rezeptHinzufuegen(GegenstandID::HOLZ, GegenstandID::HOLZ, GegenstandID::PLANKE, 2, RezeptQuelle::WERKBANK);
    rezeptHinzufuegen(GegenstandID::HOLZ, GegenstandID::STEIN, GegenstandID::AXT, 1, RezeptQuelle::WERKBANK);
    rezeptHinzufuegen(GegenstandID::PLANKE, GegenstandID::STEIN, GegenstandID::SCHAUFEL, 1, RezeptQuelle::WERKBANK);
    rezeptHinzufuegen(GegenstandID::PLANKE, GegenstandID::SEIL, GegenstandID::FLOSS, 1, RezeptQuelle::WERKBANK);
    rezeptHinzufuegen(GegenstandID::HOLZ, GegenstandID::SEIL, GegenstandID::FACKEL, 2, RezeptQuelle::WERKBANK);
    rezeptHinzufuegen(GegenstandID::PLANKE, GegenstandID::PLANKE, GegenstandID::ZAUN, 3, RezeptQuelle::WERKBANK);
    rezeptHinzufuegen(GegenstandID::PLANKE, GegenstandID::AXT, GegenstandID::TRUHE, 1, RezeptQuelle::WERKBANK);
}

const GegenstandInfo& GegenstandDaten::getInfo(GegenstandID id) const {
    static GegenstandInfo leer;
    auto it = registry.find(static_cast<int>(id));
    if (it != registry.end()) return it->second;
    return leer;
}

std::vector<Rezept> GegenstandDaten::getRezepteFuer(RezeptQuelle quelle) const {
    std::vector<Rezept> ergebnis;
    for (const auto& r : rezepte) {
        if (r.quelle == quelle) ergebnis.push_back(r);
    }
    return ergebnis;
}

void GegenstandDaten::registrieren(GegenstandID id, const std::string& name,
                                    const std::string& iconPfad, int maxStapel) {
    GegenstandInfo info;
    info.id = id;
    info.name = name;
    info.iconPfad = iconPfad;
    info.iconTextur = Kern::LoadTexture(iconPfad.c_str(), true);
    info.maxStapel = maxStapel;
    registry[static_cast<int>(id)] = info;
}

void GegenstandDaten::rezeptHinzufuegen(GegenstandID e1, GegenstandID e2,
                                         GegenstandID aus, int anzahl,
                                         RezeptQuelle quelle) {
    rezepte.push_back({e1, e2, aus, anzahl, quelle});
}
