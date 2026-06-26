//
// Created by Alexey Pravilov on 11/06/2026.
//

#include "unterwasserszeneWesen.h"

#include <GLFW/glfw3.h>
#include <gtc/matrix_transform.hpp>

#include "dieWesen/megalodon.h"
#include "dieWesen/ui/cinematicBars.h"
#include "werkzeuge/himmelsboxWesen.h"

class SeaweedBatch : public Wesen {
public:
    void prepareUniforms() const override {
        Kern::setUniform(material.shader, "alphaCutoff", 0.1f);
        Kern::setUniform(material.shader, "u_swayTime", static_cast<float>(glfwGetTime()));
    }
};
#include "dieWesen/earth.h"
#include "dieWesen/island.h"
#include "dieWesen/jellyfish.h"
#include "dieWesen/crab.h"
#include "dieWesen/turtle.h"
#include "dieWesen/flockManager.h"
#include "dieWesen/path3d.h"
#include "dieWesen/pathFollower.h"
#include "dieWesen/oceanFloor.h"
#include "dieWesen/player.h"
#include "dieWesen/raft.h"
#include "dieWesen/tree.h"
#include "dieWesen/wasser.h"
#include "dieWesen/ui/hudPanel.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/ui/worldspaceUI.h"
#include "werkzeuge/visual/worldEnvironment.h"
#include "unterwasserszeneProps.h"
#include "dieWesen/bottle.h"
#include "dieWesen/rock.h"
#include "dieWesen/thunderstorm.h"
#include "dieWesen/unterwasserszeneAudioHelper.h"
#include "dieWesen/ui/questCompletedBanner.h"
#include "dieWesen/ui/questHUD.h"
#include "dieWesen/ui/inventarHUD.h"
#include "dieWesen/ui/handwerkHUD.h"
#include "dieWesen/ui/werkbankHUD.h"
#include "dieWesen/ui/ofenHUD.h"
#include "dieWesen/ui/ausruestungsLeiste.h"
#include "dieWesen/aufhebbar.h"
#include "dieWesen/chest.h"
#include "dieWesen/coral.h"
#include "dieWesen/leviathan.h"
#include "dieWesen/seaweed.h"
#include "dieWesen/lightsaber.h"
#include "dieWesen/ui/fadeOverlay.h"
#include "szenes/raumschiffInnenWesen.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/visual/tween.h"
#include "sceneManager.h"

extern Kamera kamera;
#include "dieWesen/unterwasserszeneQuests.h"
#include "werkzeuge/audio/musicManager.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/modelManager.h"
#include "werkzeuge/inventar.h"
#include "werkzeuge/visual/questManager.h"

void UnterwasserszeneWesen::init() {
    addChild(new HimmelsboxWesen({
        "assets/textures/skybox/px.png",
        "assets/textures/skybox/nx.png",
        "assets/textures/skybox/py.png",
        "assets/textures/skybox/ny.png",
        "assets/textures/skybox/pz.png",
        "assets/textures/skybox/nz.png"
    }));

    addChild(new UnterwasserszeneAudioHelper());
    FishFlock* flock = new FishFlock();
    Path3D* fishPath = new Path3D();
    addChild(new Earth());
    addChild(new OceanFloor());
    addChild(new Wasser());
    addChild(new Island());
    addChild(new Player());
    // auto* leviathan = new Leviathan();
    // leviathan->transform.position = glm::vec3(0.0f, -120.0f, 0.0f);
    // leviathan->transform.scale = glm::vec3(4.0f);
    // addChild(leviathan);

    // JELLYFISHES
    // auto * jellyfish = new Jellyfish();
    // addChild(jellyfish);
    Path3D* jellyPath = new Path3D();
    jellyPath->transform.position = glm::vec3(-650.0f, -10.0f, -175.0f);
    jellyPath->generateCircle(25.0f, 100);
    addChild(jellyPath);

    for (int i = 0; i < 4; i++) {
        PathFollower* carrier = new PathFollower();
        carrier->targetPath = jellyPath;
        carrier->moveSpeed = 1.5f;

        carrier->currentNodeIndex = i * 25;
        carrier->transform.position = jellyPath->frames[carrier->currentNodeIndex].position;

        Jellyfish* jelly = new Jellyfish();

        jelly->transform.scale = glm::vec3(1.5f + (rand() % 100 / 100.0f));
        carrier->addChild(jelly);
        addChild(carrier);
    }

    Path3D * megalodonPath = new Path3D();
    megalodonPath->transform.position = glm::vec3(-141.0f, -240.0f, -704.0f);
    megalodonPath->generateCircle(25.0f, 100);
    addChild(megalodonPath);

    PathFollower* carrier = new PathFollower();
    carrier->targetPath = megalodonPath;
    carrier->moveSpeed = 10.0f;

    carrier->currentNodeIndex = 0;
    carrier->transform.position = megalodonPath->frames[carrier->currentNodeIndex].position;
    addChild(carrier);
    auto * megalodon = new Megalodon();
    megalodon->followTarget = carrier;
    addChild(megalodon);

    // FISHES
    // fishPath->transform.position = glm::vec3(-700.0f, -10.0f, -210.0f);
    fishPath->transform.position = glm::vec3(-511.0f, -14.0f, -174.0f);
    fishPath->generateFigureEight(40.0f, 10.0f, 40.0f);
    addChild(fishPath);
    flock->transform.position = fishPath->transform.position;
    flock->targetPath = fishPath;
    // flock->transform.position = glm::vec3(-12.f, -10.f, 0.f);
    addChild(flock);

    // Crab* crab = new Crab(2);
    // crab->transform.position = glm::vec3(-684.0f, 31.5f, -224.0f);
    // addChild(crab);

    // Turtle
    Path3D* turtlePath = new Path3D();
    turtlePath->transform.position = glm::vec3(-391.0f, -12.0f, -154.0f);
    turtlePath->generateFigureEight(40.0f, 10.0f, 40.0f);
    addChild(turtlePath);

    for (int i = 0; i < 4; i++) {
        PathFollower* carrier = new PathFollower();
        carrier->targetPath = turtlePath;
        carrier->moveSpeed = 3.5f;

        carrier->currentNodeIndex = i * 25;
        carrier->transform.position = turtlePath->frames[carrier->currentNodeIndex].position;

        int currentType = (i % 2) + 1;
        Turtle* turtle = new Turtle(currentType);

        turtle->transform.scale = glm::vec3(0.8f + (rand() % 100 / 250.0f));

        carrier->addChild(turtle);
        addChild(carrier);
    }

    auto * bottle = new Bottle();
    bottle->transform.position = glm::vec3(-580.0f, 2.0f, -180.0f);
    addChild(bottle);

    auto * worldEnv = new WorldEnvironment();
    worldEnv->init();
    worldEnv->params.sunEnergy = 4.0f;
    worldEnv->params.sunDirection = glm::vec3(0.004f, 0.055f, 0.998f);
    worldEnv->params.fogColor = glm::vec3(0.0f, 0.05f, 0.15f);
    worldEnv->params.heightFogColor = glm::vec3(0.0f, 0.22f, 0.28f);
    worldEnv->params.fogDensity = 0.025f;
    worldEnv->params.depthDimmingCoefficient = 0.01;
    worldEnv->params.bloomThreshold = 0.0f;
    worldEnv->params.bloomIntensity = 3.0f;
    addChild(worldEnv);

    auto* questHUD = new QuestHUD();
    addChild(questHUD);

    static bool startItemsGegeben = false;
    if (!startItemsGegeben) {
        Inventar::getInstance().hinzufuegen(GegenstandID::HOLZAXT, 1);
        Inventar::getInstance().hinzufuegen(GegenstandID::MINIUBOOT, 1);
        Inventar::getInstance().hinzufuegen(GegenstandID::HAUS, 1);
        Inventar::getInstance().hinzufuegen(GegenstandID::SUSHI, 1);
        Inventar::getInstance().hinzufuegen(GegenstandID::GLAS, 1);
        startItemsGegeben = true;
    }

    addChild(new InventarHUD());
    addChild(new HandwerkHUD());
    addChild(new WerkbankHUD());
    addChild(new OfenHUD());
    addChild(new AusruestungsLeiste());

    auto* seilPickup = new Aufhebbar(GegenstandID::SEIL, 2);
    seilPickup->transform.position = glm::vec3(-695.0f, 35.0f, -215.0f);
    addChild(seilPickup);

    struct SteinSpawn { float x, y, z; int anzahl; };
    SteinSpawn steinPositionen[] = {
        {-703.0f, 30.0f, -218.0f, 2},
        {-690.0f, 23.0f, -230.0f, 1},
        {-728.0f, 24.0f, -222.0f, 2},
        {-672.0f, 22.0f, -234.0f, 1},
        {-714.0f, 22.0f, -200.0f, 1},
        {-690.0f, 22.0f, -245.0f, 2},
        {-720.0f, 21.0f, -248.0f, 1},
        {-675.0f, 21.0f, -208.0f, 1},
    };

    for (const auto& sp : steinPositionen) {
        auto* stein = new Aufhebbar(GegenstandID::STEIN, sp.anzahl);
        stein->transform.position = glm::vec3(sp.x, sp.y, sp.z);
        addChild(stein);
    }

    struct SandSpawn { float x, y, z; int anzahl; };
    SandSpawn sandPositionen[] = {
        {-639.0f, 11.0f, -201.0f, 2},
        {-666.0f, 11.0f, -273.0f, 1},
        {-735.0f, 11.5f, -186.0f, 2},
        {-765.0f, 11.0f, -229.0f, 1},
        {-692.0f, 10.5f, -168.0f, 1},
        {-722.0f, 11.5f, -279.0f, 2},
    };

    for (const auto& sp : sandPositionen) {
        auto* sand = new Aufhebbar(GegenstandID::SAND, sp.anzahl);
        sand->transform.position = glm::vec3(sp.x, sp.y, sp.z);
        addChild(sand);
    }

    auto * chest = new Chest();
    chest->transform.position = glm::vec3(-690.0f, 24.0f, -225.0f);
    addChild(chest);

    auto* schatzMarkierung = new Wesen();
    schatzMarkierung->loadModel("assets/models/cube.obj");
    schatzMarkierung->material.albedo = Kern::LoadTexture("assets/textures/laser_rot.png");
    schatzMarkierung->material.emission = Kern::LoadTexture("assets/textures/laser_rot.png");
    schatzMarkierung->material.shader = ShaderManager::getInstance().getShader("default");
    schatzMarkierung->transform.scale = glm::vec3(2.0f);
    schatzMarkierung->transform.position = glm::vec3(-690.0f, 70.0f, -225.0f);
    addChild(schatzMarkierung);

    auto sceneData = getUnterwasserProps();
    for (const auto& [className, transforms] : sceneData) {
        for (const auto& t : transforms) {
            Wesen * entity = nullptr;
            if (className == "rock") {
                entity = new Rock(t.position, t.rotation, t.scale);
            }
            if (entity != nullptr) {
                addChild(entity);
            }
        }
    }

    addChild(new UnterwasserszeneQuests());
    addChild(new Thunderstorm());
    addChild(new CinematicBars());
    //addChild(new UnterwasserszeneVisualHelper());

    constexpr float mapSize = 2000.0f;
    constexpr float cellSize = 8.0f;
    constexpr float halfCell = cellSize * 0.5f;

    int gridDim = static_cast<int>(mapSize / cellSize);

    std::vector<glm::mat4> seaweedMatrices;

    for (int x = -gridDim / 2; x < gridDim / 2; x++) {
        for (int z = -gridDim / 2; z < gridDim / 2; z++) {

            if (Random::range(0.0f, 1.0f) > 0.20f) {
                continue;
            }

            float cellX = static_cast<float>(x) * cellSize;
            float cellZ = static_cast<float>(z) * cellSize;

            float rx = cellX + Random::range(-halfCell, halfCell);
            float rz = cellZ + Random::range(-halfCell, halfCell);

            const auto& islands = GroupManager::getInstance().getEntitiesInGroup("Island");
            if (!islands.empty()) {
                float islandH = dynamic_cast<Island*>(islands[0])->getHeight(rx, rz);
                if (islandH > 0.0f) continue;
            }

            const auto& floors = GroupManager::getInstance().getEntitiesInGroup("OceanFloor");
            if (floors.empty()) continue;
            OceanFloor* floor = dynamic_cast<OceanFloor*>(floors[0]);
            if (floor == nullptr) continue;
            float h = floor->getHeight(rx, rz);
            if (h > -2.0f || h < -80.0f) continue;
            float ry = h - 1.0f;

            glm::vec3 pos(rx, ry, rz);

            glm::quat yawRot = glm::angleAxis(Random::range(0.0f, 6.283f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat tiltRot = glm::angleAxis(Random::range(-0.15f, 0.15f), glm::vec3(1.0f, 0.0f, 0.0f))
                              * glm::angleAxis(Random::range(-0.15f, 0.15f), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::quat rot = yawRot * tiltRot;

            float spawnChoice = Random::range(0.0f, 1.0f);

            if (spawnChoice < 0.90f) {
                float s = Random::range(2.0f, 6.0f);
                float sx = s * Random::range(0.7f, 1.3f);
                float sz = s * Random::range(0.7f, 1.3f);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos)
                                * glm::mat4_cast(rot)
                                * glm::scale(glm::mat4(1.0f), glm::vec3(sx, s, sz));
                seaweedMatrices.push_back(model);
            }
            else {
                int randomSeed = static_cast<int>(Random::range(0.0f, 10000.0f));
                float cs = Random::range(0.5f, 2.5f);
                float csx = cs * Random::range(0.6f, 1.4f);
                float csz = cs * Random::range(0.6f, 1.4f);
                auto* entity = new Coral(pos, rot, glm::vec3(csx, cs, csz), randomSeed);

                float colorRoll = Random::range(0.0f, 1.0f);
                if (colorRoll < 0.25f) {
                    entity->material.bloomStrength = Random::range(0.8f, 2.0f);
                } else if (colorRoll < 0.50f) {
                    entity->material.bloomStrength = Random::range(0.3f, 0.8f);
                }
                addChild(entity);
            }
        }
    }

    if (!seaweedMatrices.empty()) {
        auto* seaweedBatch = new SeaweedBatch();
        CachedModel seaweedModel = ModelManager::getInstance().getModel("assets/models/seaweed.obj");
        seaweedBatch->mesh = seaweedModel.mesh;
        seaweedBatch->material.shader = ShaderManager::getInstance().loadShader(
            "seaweed", "assets/shaders/seaweed.vert", "assets/shaders/default.frag");
        seaweedBatch->material.albedo = Kern::LoadTexture("assets/textures/seaweed.png");
        seaweedBatch->material.opacity = Kern::LoadTexture("assets/textures/seaweed.png");
        seaweedBatch->material.doubleSided = true;
        seaweedBatch->material.isInstanced = static_cast<int>(seaweedMatrices.size());
        addChild(seaweedBatch);
        seaweedBatch->mesh.setupInstanceBuffer(seaweedMatrices);
    }

    // --- Leviathan cutscene (triggered from the ship scene) ---
    if (RaumschiffInnenWesen::cutscenePhase == 1) {
        leviathanCutscene = true;
        cutsceneKameraAktiv = true;

        // Disable the player
        const auto& spieler = getNodesInGroup("spielerInsel");
        if (!spieler.empty()) {
            auto* p = dynamic_cast<Player*>(spieler[0]);
            if (p) p->setActive(false);
        }

        // Complete all quests so the HUD is clean
        QuestManager::getInstance().cleanup();

        // Hide quest HUD
        const auto& questHuds = getNodesInGroup("QuestManager");
        for (auto* q : questHuds) q->visible = false;

        // Play the ship scene music
        MusicManager::getInstance().playMusic("assets/audio/beatit.mp3", 1.5f, true);

        // Add cinematic bars
        auto* bars = new CinematicBars();
        addChild(bars);
        bars->setEnabled(true);

        // Fade in from black
        auto* fadeIn = new FadeOverlay();
        addChild(fadeIn);
        fadeIn->sofort(1.0f);
        fadeIn->fadeOut(1.5f);

        // Spawn leviathan deep underwater with lightsabers
        cutsceneLeviathan = new Wesen();
        cutsceneLeviathan->loadModel("assets/models/leviathan.obj");
        cutsceneLeviathan->material.albedo = Kern::LoadTexture("assets/textures/leviathan_albedo.png");
        cutsceneLeviathan->material.emission = Kern::LoadTexture("assets/textures/leviathan_emissive.png");
        cutsceneLeviathan->material.normal = Kern::LoadTexture("assets/textures/leviathan_normal.png");
        cutsceneLeviathan->material.bloomStrength = 0.3f;
        cutsceneLeviathan->material.shader = ShaderManager::getInstance().getShader("default");
        cutsceneLeviathan->transform.position = glm::vec3(-500.0f, -60.0f, -100.0f);
        cutsceneLeviathan->transform.scale = glm::vec3(4.0f);
        addChild(cutsceneLeviathan);

        const std::string saberFarben[4] = {"red", "purple", "green", "blue"};
        glm::vec3 saberPos[4] = {
            glm::vec3( 6.0f,  4.0f, 22.0f), glm::vec3( 8.0f, -6.0f, 18.0f),
            glm::vec3(-6.0f,  4.0f, 22.0f), glm::vec3(-8.0f, -6.0f, 18.0f),
        };
        for (int i = 0; i < 4; i++) {
            auto* dummy = new Wesen();
            auto* saber = new Lightsaber("assets/textures/lightsaber_" + saberFarben[i] + ".png");
            saber->dir = (i % 2 == 0) ? 1 : -1;
            cutsceneLeviathan->addChild(dummy);
            dummy->addChild(saber);
            dummy->transform.position = saberPos[i];
        }

        // Camera above open water, looking down at the leviathan
        cameraZielPos = glm::vec3(-500.0f, 15.0f, -20.0f);
        cameraBlickZiel = glm::vec3(-500.0f, -60.0f, -100.0f);

        createTween()
            ->tweenInterval(2.0f)
            // Leviathan swims upward
            ->tweenProperty(&cutsceneLeviathan->transform.position.y, -20.0f, 3.0f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&cameraBlickZiel.y, -20.0f, 3.0f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&cameraZielPos.y, 10.0f, 3.0f, EaseType::EASE_OUT_SINE)
            // Breaches the surface with a roll!
            ->tweenProperty(&cutsceneLeviathan->transform.position.y, 40.0f, 2.0f, EaseType::EASE_IN_OUT_SINE)
            ->parallel()
            ->tweenProperty(&cameraBlickZiel.y, 40.0f, 2.0f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&cameraZielPos.y, 50.0f, 2.0f, EaseType::EASE_OUT_SINE)
            ->parallel()
            ->tweenProperty(&leviathanEuler.x, 360.0f, 2.0f, EaseType::EASE_IN_OUT_SINE)
            // Flies up into the sky, spiraling
            ->tweenProperty(&cutsceneLeviathan->transform.position.y, 500.0f, 3.0f, EaseType::EASE_IN)
            ->parallel()
            ->tweenProperty(&cameraBlickZiel.y, 500.0f, 3.0f, EaseType::EASE_IN)
            ->parallel()
            ->tweenProperty(&cameraZielPos.y, 100.0f, 3.0f, EaseType::EASE_IN)
            ->parallel()
            ->tweenProperty(&leviathanEuler.z, 720.0f, 3.0f, EaseType::EASE_IN)
            ->parallel()
            ->tweenProperty(&leviathanEuler.y, -45.0f, 3.0f, EaseType::EASE_IN_OUT_SINE)
            ->tweenInterval(0.5f)
            // Fade to black, switch to ship scene for Death Star phase
            ->tweenCallback([this]() {
                auto* fadeOut = new FadeOverlay();
                addChild(fadeOut);
                fadeOut->fadeIn(1.0f, []() {
                    RaumschiffInnenWesen::cutscenePhase = 2;
                    Scene::requestSceneSwitch(3);
                });
            });
    }
}

void UnterwasserszeneWesen::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (cutsceneKameraAktiv) {
        const float t = 1.0f - glm::exp(-3.0f * deltaTime);
        cameraTransform.position = glm::mix(cameraTransform.position, cameraZielPos, t);
        cameraTransform.lookAt(cameraBlickZiel, glm::vec3(0.0f, 1.0f, 0.0f));

        if (cutsceneLeviathan && glm::length2(leviathanEuler) > 0.001f) {
            cutsceneLeviathan->transform.rotation = glm::quat(glm::radians(leviathanEuler));
        }
    }
}
