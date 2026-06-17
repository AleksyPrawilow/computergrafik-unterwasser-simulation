#include "weltraumszeneWesen.h"

#include "dieWesen/asteroid.h"
#include "dieWesen/feindschiff.h"
#include "dieWesen/raumschiff.h"
#include "dieWesen/ui/weltraumHudPanel.h"
#include "werkzeuge/himmelsboxWesen.h"
#include "werkzeuge/random.h"
#include "werkzeuge/visual/worldEnvironment.h"

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

    addChild(new Raumschiff());
    addChild(new WeltraumHudPanel());

    for (int i = 0; i < 20; i++) {
        auto * ast = new Asteroid(
            Random::range(30.0f, 100.0f),
            Random::range(0.01f, 0.05f),
            Random::range(0.5f, 2.0f)
        );
        ast->transform.scale = glm::vec3(Random::range(1.0f, 5.0f));
        ast->transform.position.y = Random::range(-20.0f, 20.0f);
        addChild(ast);
    }

    for (int i = 0; i < 12; i++) {
        auto * feind = new Feindschiff();
        float winkel = static_cast<float>(i) * 6.28f / 12.0f;
        float radius = Random::range(40.0f, 120.0f);
        float hoehe = Random::range(-30.0f, 30.0f);
        glm::vec3 startPos(glm::cos(winkel) * radius, hoehe, glm::sin(winkel) * radius);
        feind->transform.position = startPos;

        for (int w = 0; w < 4; w++) {
            float wWinkel = winkel + static_cast<float>(w + 1) * 1.57f;
            float wRadius = Random::range(30.0f, 100.0f);
            feind->wegpunkte.push_back(glm::vec3(
                glm::cos(wWinkel) * wRadius,
                Random::range(-25.0f, 25.0f),
                glm::sin(wWinkel) * wRadius
            ));
        }
        addChild(feind);
    }
}
