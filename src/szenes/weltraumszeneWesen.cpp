#include "weltraumszeneWesen.h"

#include "dieWesen/asteroid.h"
#include "dieWesen/feindschiff.h"
#include "dieWesen/raumschiff.h"
#include "dieWesen/ui/questCompletedBanner.h"
#include "dieWesen/ui/weltraumHudPanel.h"
#include "dieWesen/ui/fadeOverlay.h"
#include "dieWesen/ui/cinematicBars.h"
#include "werkzeuge/himmelsboxWesen.h"
#include "werkzeuge/random.h"
#include "werkzeuge/visual/worldEnvironment.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/musicManager.h"
#include "werkzeuge/groupManager.h"
#include "werkzeuge/input.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/renderWerkzeuge.h"
#include "sceneManager.h"
#include "dieWesen/ui/starWarsIntro.h"

extern bool cursorDisabled;

int WeltraumszeneWesen::abschuesse = 0;

static glm::vec3 sichereSpawnPosition(float minRadius, float maxRadius, float minHoehe, float maxHoehe, float sicherheitsAbstand) {
    const auto& spielerGruppe = GroupManager::getInstance().getEntitiesInGroup("spieler");
    glm::vec3 spielerPos(0.0f);
    if (!spielerGruppe.empty())
        spielerPos = spielerGruppe[0]->getGlobalTransform().position;

    for (int versuch = 0; versuch < 20; versuch++) {
        float winkel = Random::range(0.0f, 6.28f);
        float radius = Random::range(minRadius, maxRadius);
        float hoehe = Random::range(minHoehe, maxHoehe);
        glm::vec3 pos(glm::cos(winkel) * radius, hoehe, glm::sin(winkel) * radius);
        if (glm::distance(pos, spielerPos) >= sicherheitsAbstand)
            return pos;
    }
    return spielerPos - glm::vec3(0.0f, 0.0f, maxRadius);
}

void WeltraumszeneWesen::init() {
    addChild(new HimmelsboxWesen({
        "assets/textures/skybox_weltraum/px.png",
        "assets/textures/skybox_weltraum/nx.png",
        "assets/textures/skybox_weltraum/py.png",
        "assets/textures/skybox_weltraum/ny.png",
        "assets/textures/skybox_weltraum/pz.png",
        "assets/textures/skybox_weltraum/nz.png"
    }));

    auto * umwelt = new WorldEnvironment();
    umwelt->params.sunDirection = glm::normalize(glm::vec3(0.3f, 0.8f, 0.2f));
    umwelt->params.sunColor = glm::vec3(1.0f, 1.0f, 0.98f);
    umwelt->params.sunEnergy = 2.0f;
    umwelt->params.ambientColor = glm::vec3(0.15f, 0.15f, 0.2f);
    umwelt->params.ambientEnergy = 0.05f;
    umwelt->params.fogEnabled = false;
    umwelt->params.heightFogEnabled = false;
    umwelt->params.causticsEnabled = false;
    umwelt->params.depthDimmingEnabled = false;
    addChild(umwelt);

    raumschiff = new Raumschiff();
    addChild(raumschiff);
    addChild(new WeltraumHudPanel());

    raumschiff->onDeath = [this]() {
        alleFeindeEntfernen();
        spawnDelay->startTimer(2.0f, [this]() {
            welleStarten(aktuelleWelle);
        });
    };

    wellenTimer = new Timer();
    addChild(wellenTimer);

    spawnDelay = new Timer();
    addChild(spawnDelay);

    bordTimer = new Timer();
    addChild(bordTimer);

    bordPrompt = new UILabel();
    bordPrompt->setText("Press [E] to board the wreck", 40.0f);
    bordPrompt->color = glm::vec4(1.0f, 0.9f, 0.4f, 1.0f);
    bordPrompt->expansion = UIExpansion::CENTER;
    addChild(bordPrompt);
    bordPrompt->visible = false;

    fadeOverlay = new FadeOverlay();
    addChild(fadeOverlay);

    cinematicBars = new CinematicBars();
    addChild(cinematicBars);

    addChild(new StarWarsIntro());
    // MusicManager::getInstance().playMusic("assets/audio/beatit.mp3", 2.0f, true);
}

void WeltraumszeneWesen::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (Input::isKeyJustPressed(GLFW_KEY_G) && aktuelleWelle < 2) {
        alleFeindeEntfernen();
        welleStarten(2);
    }

    bordenPruefen(window);
}

void WeltraumszeneWesen::bordenPruefen(GLFWwindow* window) {
    if (bordVorgang) return;

    // Only once the boss corpse exists (wave 3 cleared) and the player ship is around.
    const auto& leichen = getNodesInGroup("bossLeiche");
    if (leichen.empty() || raumschiff == nullptr) {
        if (bordPrompt) bordPrompt->visible = false;
        return;
    }

    glm::vec3 spielerPos = raumschiff->getGlobalTransform().position;
    glm::vec3 wrackPos = leichen[0]->getGlobalTransform().position;
    // Boss corpse can be at any Y in space -> full 3D distance, never flattened.
    float abstand = glm::distance(spielerPos, wrackPos);

    bool inReichweite = abstand < 160.0f;

    glm::vec2 viewport = Kern::GetViewportSize() / UIElement::dpiScale;
    bordPrompt->transform.position = glm::vec3(viewport.x * 0.5f, viewport.y * 0.75f, 0.0f);
    bordPrompt->visible = inReichweite;

    if (inReichweite && Input::isKeyJustPressed(GLFW_KEY_E)) {
        bordVorgang = true;
        bordPrompt->visible = false;
        raumschiff->setIstAktiv(false);

        // Cinematic bars slide in, then fade to black, then switch scene.
        cinematicBars->setEnabled(true);

        bordTimer->startTimer(0.5f, [this]() {
            fadeOverlay->fadeIn(0.8f, []() {
                Scene::requestSceneSwitch(3);
            });
        });
    }
}

void WeltraumszeneWesen::alleFeindeEntfernen() {
    welleAktiv = false;
    wellenTimer->stopTimer();

    auto feinde = getNodesInGroup("feinde");
    for (auto* f : feinde) f->queueDestroy();

    auto asteroiden = getNodesInGroup("asteroiden");
    for (auto* a : asteroiden) a->queueDestroy();

    auto torpedos = getNodesInGroup("torpedos");
    for (auto* t : torpedos) t->queueDestroy();

    auto laser = getNodesInGroup("feindlaser");
    for (auto* l : laser) l->queueDestroy();

    auto pickups = getNodesInGroup("pickups");
    for (auto* p : pickups) p->queueDestroy();

    auto leichen = getNodesInGroup("bossLeiche");
    for (auto* l : leichen) l->queueDestroy();
}

void WeltraumszeneWesen::welleStarten(int welle) {
    aktuelleWelle = welle;
    abschuesse = 0;
    if (raumschiff) {
        raumschiff->vollHeilen();
        raumschiff->raketenAuffuellen();
    }

    if (welle > 2) {
        addChild(new QuestCompletedBanner("", "V I C T O R Y !"));
        return;
    }

    std::string wellenName = "W A V E   " + std::to_string(welle + 1);
    addChild(new QuestCompletedBanner("Get ready!", wellenName));

    spawnDelay->startTimer(2.0f, [this, welle]() {
        if (raumschiff) raumschiff->setSpawnSchutz(3.0f);
        switch (welle) {
            case 0: spawnWave(15, 8, false); break;
            case 1: spawnWave(20, 12, false); break;
            case 2: spawnWave(10, 6, true); break;
            default: break;
        }
        welleAktiv = true;
        wellenTimer->startTimer(1.0f, [this]() { wellenPruefung(); });
    });
}

void WeltraumszeneWesen::wellenPruefung() {
    if (!welleAktiv) return;

    const auto& feinde = getNodesInGroup("feinde");
    if (feinde.empty()) {
        welleAktiv = false;
        if (aktuelleWelle < 2) {
            aktuelleWelle++;
            welleStarten(aktuelleWelle);
        } else {
            welleStarten(3);
        }
        return;
    }

    wellenTimer->startTimer(1.0f, [this]() { wellenPruefung(); });
}

void WeltraumszeneWesen::spawnWave(int numA, int numB, bool mitBoss) {
    for (int i = 0; i < numA; i++) {
        auto * ast = new Asteroid(
            Random::range(60.0f, 250.0f),
            Random::range(0.01f, 0.05f),
            Random::range(0.5f, 2.0f)
        );
        ast->transform.scale = glm::vec3(Random::range(10.0f, 25.0f));
        ast->transform.position.y = Random::range(-40.0f, 40.0f);
        addChild(ast);
    }

    for (int i = 0; i < numB; i++) {
        auto * feind = new Feindschiff();
        constexpr float sicherheit = 60.0f;
        glm::vec3 startPos = sichereSpawnPosition(100.0f, 300.0f, -60.0f, 60.0f, sicherheit);
        feind->transform.position = startPos;

        float patrolChance = (aktuelleWelle == 0) ? 0.4f : (aktuelleWelle == 1) ? 0.3f : 0.2f;
        float jaegerChance = (aktuelleWelle == 0) ? 0.7f : (aktuelleWelle == 1) ? 0.65f : 0.6f;
        float typRoll = Random::range(0.0f, 1.0f);
        if (typRoll < patrolChance)
            feind->typ = FeindTyp::PATROUILLE;
        else if (typRoll < jaegerChance)
            feind->typ = FeindTyp::JAEGER;
        else
            feind->typ = FeindTyp::ORBITER;

        if (aktuelleWelle >= 1) {
            feind->bewegungsGeschwindigkeit = 8.0f;
            feind->schussIntervall = 1.5f;
        }
        if (aktuelleWelle >= 2) {
            feind->leben = 2.0f;
            feind->bewegungsGeschwindigkeit = 10.0f;
            feind->schussIntervall = 1.2f;
        }

        float winkel = glm::atan(startPos.z, startPos.x);
        for (int w = 0; w < 4; w++) {
            float wWinkel = winkel + static_cast<float>(w + 1) * 1.57f;
            float wRadius = Random::range(60.0f, 250.0f);
            feind->wegpunkte.emplace_back(
                glm::cos(wWinkel) * wRadius,
                Random::range(-50.0f, 50.0f),
                glm::sin(wWinkel) * wRadius
            );
        }
        addChild(feind);

        switch (feind->typ) {
            case FeindTyp::PATROUILLE:
                feind->material.emission = Kern::LoadTexture("assets/textures/emission_cyan.png");
                break;
            case FeindTyp::JAEGER:
                feind->material.emission = Kern::LoadTexture("assets/textures/laser_rot.png");
                break;
            case FeindTyp::ORBITER:
                feind->material.emission = Kern::LoadTexture("assets/textures/emission_lila.png");
                break;
        }
    }

    if (mitBoss) {
        auto * boss = new Feindschiff();
        boss->leben = 20.0f;
        boss->laserSchaden = 30.0f;
        constexpr float bossSicherheit = 250.0f;
        glm::vec3 bossPos = sichereSpawnPosition(350.0f, 500.0f, -40.0f, 40.0f, bossSicherheit);
        boss->transform.position = bossPos;
        float bossWinkel = glm::atan(bossPos.z, bossPos.x);
        for (int w = 0; w < 4; w++) {
            float wWinkel = bossWinkel + static_cast<float>(w + 1) * 1.57f;
            float wRadius = Random::range(100.0f, 300.0f);
            boss->wegpunkte.emplace_back(
                glm::cos(wWinkel) * wRadius,
                Random::range(-40.0f, 40.0f),
                glm::sin(wWinkel) * wRadius
            );
        }
        addChild(boss);
        boss->transform.scale = glm::vec3(100.0f);
        boss->boundingRadius = 120.0f;
        boss->material.albedo = Kern::LoadTexture("assets/textures/boss_gold.png");
        boss->material.emission = Kern::LoadTexture("assets/textures/boss_gold.png");
        boss->material.bloomStrength = 0.2f;
        boss->laserGroesse = glm::vec3(0.4f, 0.4f, 8.0f);
        boss->laserOffset = 30.0f;
        boss->bewegungsGeschwindigkeit = 25.0f;
        boss->drehGeschwindigkeit = 1.0f;
        boss->schussIntervall = 1.0f;
    }
}
