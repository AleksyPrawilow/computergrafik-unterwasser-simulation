#include "platzierbaresObjekt.h"
#include "werkzeuge/modelManager.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/lightManager.h"

PlatzierbaresObjekt::PlatzierbaresObjekt(GegenstandID typ) : typ(typ) {}

void PlatzierbaresObjekt::init() {
    isCollidable = true;
    const auto& info = GegenstandDaten::getInstance().getInfo(typ);

    if (typ == GegenstandID::HAUS) {
        name = "haus";
        transform.scale = glm::vec3(1.0f);
        addToGroup("haus");
        buildHouse();
        return;
    }

    loadModel("assets/models/cube.obj");
    material.albedo = info.iconTextur;
    material.shader = ShaderManager::getInstance().getShader("default");

    if (typ == GegenstandID::WERKBANK) {
        name = "werkbank";
        transform.scale = glm::vec3(1.0f, 0.8f, 1.0f);
        addToGroup("werkbank");
    } else if (typ == GegenstandID::OFEN) {
        name = "ofen";
        transform.scale = glm::vec3(0.9f, 0.9f, 0.9f);
        addToGroup("ofen");
    } else if (typ == GegenstandID::LABORKONSOLE) {
        name = "laborkonsole";
        transform.scale = glm::vec3(1.6f, 1.2f, 1.6f);
        addToGroup("schiffslabor");
    } else if (typ == GegenstandID::GLAS) {
        name = "glas";
        loadModel("assets/models/triangle.obj");
        transform.scale = glm::vec3(3.5f, 1.0f, 1.0f);
        material.normal = Kern::LoadTexture("assets/textures/water_normal.png");
        material.isTransparent = true;
        material.doubleSided = true;
        material.shader = ShaderManager::getInstance().loadShader(
            "windshield",
            "assets/shaders/default.vert",
            "assets/shaders/refract.frag"
        );
        return;
    } else if (typ == GegenstandID::ZAUN) {
        name = "zaun";
        transform.scale = glm::vec3(1.5f, 1.2f, 0.2f);
    } else if (typ == GegenstandID::TRUHE) {
        name = "truhe";
        transform.scale = glm::vec3(0.8f, 0.6f, 0.6f);
    } else if (typ == GegenstandID::FACKEL) {
        name = "fackel";
        transform.scale = glm::vec3(0.15f, 1.0f, 0.15f);
        torchLight = LightManager::getInstance().createPointLight(
            glm::vec3(1.0f, 0.7f, 0.3f), 50.0f);
    } else {
        name = "platziert";
        transform.scale = glm::vec3(0.5f);
    }
}

void PlatzierbaresObjekt::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (torchLight != nullptr) {
        torchLight->position = getGlobalTransform().position + glm::vec3(0.0f, 1.5f, 0.0f);
    }
}

Wesen* PlatzierbaresObjekt::makePanel(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale) {
    auto* panel = new Wesen();
    panel->loadModel("assets/models/raft.obj");
    panel->material.albedo = Kern::LoadTexture("assets/textures/Raft_baseColor.png");
    panel->material.normal = Kern::LoadTexture("assets/textures/Raft_normal.png");
    panel->material.roughness = Kern::LoadTexture("assets/textures/Raft_metallicRoughness.png");
    panel->material.metallic = Kern::LoadTexture("assets/textures/Raft_metallicRoughness.png");
    panel->material.shader = ShaderManager::getInstance().getShader("default");
    panel->transform.position = pos;
    panel->transform.rotation = glm::quat(glm::radians(euler));
    panel->transform.scale = scale;
    panel->isCollidable = true;
    panel->addToGroup("hauswand");
    addChild(panel);
    return panel;
}

void PlatzierbaresObjekt::buildHouse() {
    float panelScale = 0.22f;
    float w = 13.6f * panelScale * 0.85f;  // overlap panels by 15%
    float h = 24.0f * panelScale;
    float houseW = w * 3.0f;
    float houseD = w * 3.0f;
    glm::vec3 ps = glm::vec3(panelScale);

    // Back wall: 3 panels
    for (int i = 0; i < 3; i++) {
        float x = -houseW / 2.0f + w * 0.5f + i * w;
        makePanel(glm::vec3(x, h * 0.5f, houseD / 2.0f), glm::vec3(90.0f, 0.0f, 0.0f), ps);
    }

    // Front wall: 2 panels with door gap
    makePanel(glm::vec3(-houseW / 2.0f + w * 0.5f, h * 0.5f, -houseD / 2.0f), glm::vec3(90.0f, 0.0f, 0.0f), ps);
    makePanel(glm::vec3(houseW / 2.0f - w * 0.5f, h * 0.5f, -houseD / 2.0f), glm::vec3(90.0f, 0.0f, 0.0f), ps);

    // Left wall: 3 panels
    for (int i = 0; i < 3; i++) {
        float z = -houseD / 2.0f + w * 0.5f + i * w;
        makePanel(glm::vec3(-houseW / 2.0f, h * 0.5f, z), glm::vec3(90.0f, 90.0f, 0.0f), ps);
    }

    // Right wall: 3 panels
    for (int i = 0; i < 3; i++) {
        float z = -houseD / 2.0f + w * 0.5f + i * w;
        makePanel(glm::vec3(houseW / 2.0f, h * 0.5f, z), glm::vec3(90.0f, 90.0f, 0.0f), ps);
    }

    // Roof: 2 large panels leaning into each other, forming A-frame
    float roofScale = panelScale * 2.5f;
    float roofAngle = 30.0f;
    float roofY = h + 0.8f;

    makePanel(glm::vec3(-houseW * 0.25f, roofY, 0.0f),
              glm::vec3(0.0f, 0.0f, roofAngle),
              glm::vec3(roofScale * 0.8f, roofScale * 0.5f, roofScale * 0.55f));
    makePanel(glm::vec3(houseW * 0.25f, roofY, 0.0f),
              glm::vec3(0.0f, 0.0f, -roofAngle),
              glm::vec3(roofScale * 0.8f, roofScale * 0.5f, roofScale * 0.55f));
}
