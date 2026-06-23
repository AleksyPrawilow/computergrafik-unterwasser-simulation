#include "gegenstandDaten.h"
#include "textur.h"

void GegenstandDaten::init() {
    registrieren(GegenstandID::HOLZ, "Wood", "assets/textures/icon_holz.png", 64);
    registrieren(GegenstandID::STEIN, "Stone", "assets/textures/icon_stein.png", 64);
    registrieren(GegenstandID::PLANKE, "Plank", "assets/textures/icon_planke.png", 64);
    registrieren(GegenstandID::HOLZAXT, "Wooden Axe", "assets/textures/icon_holzaxt.png", 1);
    registrieren(GegenstandID::AXT, "Axe", "assets/textures/icon_axt.png", 1);
    registrieren(GegenstandID::FLASCHE, "Bottle", "assets/textures/icon_flasche.png", 16);
    registrieren(GegenstandID::SEIL, "Rope", "assets/textures/icon_seil.png", 16);
    registrieren(GegenstandID::FLOSS, "Raft", "assets/textures/icon_floss.png", 1);
    registrieren(GegenstandID::WERKBANK, "Workbench", "assets/textures/icon_werkbank.png", 1);
    registrieren(GegenstandID::SCHAUFEL, "Shovel", "assets/textures/icon_schaufel.png", 1);
    registrieren(GegenstandID::ERDE, "Dirt", "assets/textures/icon_erde.png", 64);
    registrieren(GegenstandID::FACKEL, "Torch", "assets/textures/icon_fackel.png", 16);
    registrieren(GegenstandID::ZAUN, "Fence", "assets/textures/icon_zaun.png", 16);
    registrieren(GegenstandID::TRUHE, "Chest", "assets/textures/icon_truhe.png", 1);
}

const GegenstandInfo& GegenstandDaten::getInfo(GegenstandID id) const {
    static GegenstandInfo leer;
    auto it = registry.find(static_cast<int>(id));
    if (it != registry.end()) return it->second;
    return leer;
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
