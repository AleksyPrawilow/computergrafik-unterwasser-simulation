#include "raumschiff.h"
#include "torpedo.h"

#include "../werkzeuge/textur.h"
#include "ui/fadenkreuz.h"
#include "werkzeuge/input.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void Raumschiff::init() {
    material.albedo = Kern::LoadTexture("assets/textures/raumschiff_albedo.png");
    material.roughness = Kern::LoadTexture("assets/textures/raumschiff_roughness.png");
    material.metallic = Kern::LoadTexture("assets/textures/raumschiff_metallic.png");
    material.normal = Kern::LoadTexture("assets/textures/raumschiff_normal.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );
    loadModel("assets/models/spaceship2.obj");
    transform.position = glm::vec3(0.f, 0.f, 0.f);
    transform.scale = glm::vec3(3.0f);
    boundingRadius = 4.0f;

    addToGroup("spieler");

    fadenkreuz = new Fadenkreuz();
    fadenkreuz->init(16.0f / 9.0f);
    addChild(fadenkreuz);

    for (int i = 0; i < 2; i++) {
        auto * triebwerk = new ParticleEmitter(3000);
        addChild(triebwerk);
        triebwerk->transform.position = glm::vec3(-0.2f * (i == 0 ? 1.0f : -1.0f), 0.0f, 0.8f);
        triebwerk->emitRichtung = glm::vec3(0.0f, 0.0f, 1.0f);
        triebwerk->aufstiegZiel = 0.0f;
        triebwerke[i] = triebwerk;
    }

    shootSound = new AudioPlayer("assets/audio/shoot.mp3", false, 10, false);
    addChild(shootSound);

    canShootTimer = new Timer();
    addChild(canShootTimer);
}

void Raumschiff::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    zielGeschwindigkeit = 0.0f;
    zielRollGeschwindigkeit = 0.0f;
    fadenkreuz->visible = istAktiv;

    eingabeVerarbeiten(window, deltaTime);

    constexpr float beschleunigung = 0.75f;
    const float tSpeed = 1.0f - glm::exp(-beschleunigung * deltaTime);
    tatsaechlicheGeschwindigkeit = glm::mix(tatsaechlicheGeschwindigkeit, zielGeschwindigkeit, tSpeed);

    rollsBehandeln(window, deltaTime);

    // FIX 1: Normalize the spaceship's rotation quaternion every frame
    // This prevents coordinate warping from consecutive pitch and roll operations.
    transform.rotation = glm::normalize(transform.rotation);

    if (istAktiv) {
        kameraAktualisieren(cameraTransform, deltaTime);

        // FIX 2: Normalize the camera's rotation quaternion to prevent camera drift
        cameraTransform.rotation = glm::normalize(cameraTransform.rotation);
    }

    for (auto * triebwerk : triebwerke) {
        triebwerk->active = glm::abs(tatsaechlicheGeschwindigkeit) > 1.0f;
    }
}

void Raumschiff::eingabeVerarbeiten(GLFWwindow* window, const float deltaTime) {
    if (Input::isKeyJustPressed(GLFW_KEY_V)) {
        ansichtModus = (ansichtModus == AnsichtModus::DRITTE_PERSON)
            ? AnsichtModus::ERSTE_PERSON
            : AnsichtModus::DRITTE_PERSON;
    }

    if (!istAktiv) return;

    if (Input::isKeyPressed(GLFW_KEY_W)) {
        zielGeschwindigkeit = bewegungsGeschwindigkeit;
        transform.position += transform.forward() * tatsaechlicheGeschwindigkeit * deltaTime;
    }

    if (Input::isKeyPressed(GLFW_KEY_S)) {
        zielGeschwindigkeit = rueckwaertsGeschwindigkeit;
        transform.position -= transform.forward() * tatsaechlicheGeschwindigkeit * deltaTime;
    }

    if (Input::isKeyPressed(GLFW_KEY_A)) {
        zielRollGeschwindigkeit = -winkelGeschwindigkeit;
    }
    if (Input::isKeyPressed(GLFW_KEY_D)) {
        zielRollGeschwindigkeit = winkelGeschwindigkeit;
    }

    if (Input::isKeyJustPressed(GLFW_KEY_Z)) {
        if (!isDashing) {
            isDashing = true;
            dashDir = -transform.right();
            currentDashSpeed = maxDashSpeed;
            dashRollAngle = 0.0f;
            lastDashRollAngle = 0.0f;

            createTween()
                ->tweenProperty(&currentDashSpeed, 0.0f, 0.75f, EaseType::EASE_IN_CIRC)
                ->parallel()
                ->tweenProperty(&dashRollAngle, -360.0f, 1.0f, EaseType::EASE_IN_OUT_SINE)
                ->tweenCallback([this]() {
                    isDashing = false;
                });
        }
    }

    if (Input::isKeyJustPressed(GLFW_KEY_X)) {
        if (!isDashing) {
            isDashing = true;
            dashDir = transform.right();
            currentDashSpeed = maxDashSpeed;
            dashRollAngle = 0.0f;
            lastDashRollAngle = 0.0f;

            createTween()
                ->tweenProperty(&currentDashSpeed, 0.0f, 0.75f, EaseType::EASE_IN_CIRC)
                ->parallel()
                ->tweenProperty(&dashRollAngle, 360.0f, 1.0f, EaseType::EASE_IN_OUT_SINE)
                ->tweenCallback([this]() {
                    isDashing = false;
                });
        }
    }

    if (Input::isKeyPressed(GLFW_KEY_SPACE) && parent != nullptr && canShoot) {
        canShoot = false;
        canShootTimer->startTimer(0.1f, [this]() { canShoot = true; });
        shootSound->play();
        auto * torpedo = new Torpedo();
        parent->addChild(torpedo);
        torpedo->abfeuern(
            transform.position + transform.forward() * 5.0f,
            transform.rotation
        );
    }

    if (isDashing) {
        // Calculate how many degrees the tween rotated since the last frame
        float deltaRoll = dashRollAngle - lastDashRollAngle;

        // Apply the incremental rotation around the local forward axis
        transform.roll(glm::radians(deltaRoll));

        // Update tracker
        lastDashRollAngle = dashRollAngle;
    }

    transform.position += dashDir * deltaTime * currentDashSpeed;
}

void Raumschiff::kameraAktualisieren(Transform& cameraTransform, float deltaTime) const {
    const glm::vec3 schiffPos = transform.position;
    const glm::vec3 vorwaerts = transform.forward();

    // 1. Calculate the camera up-vector (cancels visual barrel roll only in 3rd person)
    glm::vec3 cameraUp = transform.up();
    if (isDashing && ansichtModus == AnsichtModus::DRITTE_PERSON) {
        glm::quat rollCorrection = glm::angleAxis(glm::radians(-dashRollAngle), vorwaerts);
        cameraUp = glm::normalize(rollCorrection * cameraUp);
    }

    glm::vec3 zielKameraPos;
    glm::vec3 blickZiel;

    if (ansichtModus == AnsichtModus::DRITTE_PERSON) {
        zielKameraPos = schiffPos - vorwaerts * 9.0f + cameraUp * 1.5f;
        blickZiel = schiffPos + cameraUp * 0.5f;
    } else {
        zielKameraPos = schiffPos + vorwaerts * 2.1f;
        blickZiel = schiffPos + vorwaerts * 10.0f;
    }

    // 2. Apply camera updates
    if (isDashing) {
        // --- SNAP INSTANTLY DURING THE DASH ---
        // Bypasses all smooth-follow interpolation so the camera remains rigidly locked
        cameraTransform.position = zielKameraPos;
        cameraTransform.lookAt(blickZiel, cameraUp);
    } else {
        // --- SMOOTH FOLLOW DURING STANDARD FLIGHT ---
        const float kameraFolgeGeschwindigkeit = 6.0f;
        const float kameraDrehGeschwindigkeit = 8.0f;

        const float tFolge = (ansichtModus == AnsichtModus::ERSTE_PERSON)
            ? 1.0f : 1.0f - glm::exp(-kameraFolgeGeschwindigkeit * deltaTime);

        const float tDreh = (ansichtModus == AnsichtModus::ERSTE_PERSON)
            ? 1.0f : 1.0f - glm::exp(-kameraDrehGeschwindigkeit * deltaTime);

        const glm::quat aktuelleKameraRot = cameraTransform.rotation;
        cameraTransform.lookAt(blickZiel, cameraUp);
        const glm::quat zielKameraRot = cameraTransform.rotation;

        cameraTransform.position = glm::mix(cameraTransform.position, zielKameraPos, tFolge);
        cameraTransform.rotation = glm::slerp(aktuelleKameraRot, zielKameraRot, tDreh);
    }
}

void Raumschiff::rollsBehandeln(GLFWwindow* window, const float deltaTime) {
    const float tRoll = 1.0f - glm::exp(-10.0f * deltaTime);
    rollGeschwindigkeit = glm::mix(rollGeschwindigkeit, zielRollGeschwindigkeit, tRoll);
    transform.roll(rollGeschwindigkeit * deltaTime);

    double mausX, mausY;
    glfwGetCursorPos(window, &mausX, &mausY);

    if (ersteMaus) {
        letzteX = mausX;
        letzteY = mausY;
        ersteMaus = false;
    }

    double xVersatz = mausX - letzteX;
    double yVersatz = letzteY - mausY;

    letzteX = mausX;
    letzteY = mausY;

    constexpr float mausEmpfindlichkeit = 0.15f;
    xVersatz *= mausEmpfindlichkeit;
    yVersatz *= mausEmpfindlichkeit;

    const auto zielNickGeschwindigkeit = static_cast<float>(yVersatz) * static_cast<float>(istAktiv);
    const auto zielGierGeschwindigkeit = -static_cast<float>(xVersatz) * static_cast<float>(istAktiv);

    constexpr float schiffTraegheit = 8.0f;
    nickGeschwindigkeit = glm::mix(nickGeschwindigkeit, zielNickGeschwindigkeit, schiffTraegheit * deltaTime);
    gierGeschwindigkeit = glm::mix(gierGeschwindigkeit, zielGierGeschwindigkeit, schiffTraegheit * deltaTime);

    transform.pitch(nickGeschwindigkeit * deltaTime);
    transform.yaw(gierGeschwindigkeit * deltaTime);
}

void Raumschiff::schadenNehmen(float schaden) {
    kamera.addShake(0.4f, 0.25f);
    leben -= schaden;
    if (leben <= 0.0f) {
        leben = 100.0f;
        transform.position = glm::vec3(0.0f);
        transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        tatsaechlicheGeschwindigkeit = 0.0f;
    }
}
