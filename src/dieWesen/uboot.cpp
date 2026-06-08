//
// Created by Alexey Pravilov on 02/06/2026.
//
#include "uboot.h"

#include "ubootRotor.h"
#include "../werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"

void Uboot::init() {
    material.albedo = Kern::LoadTexture("assets/textures/sub_albedo.png");
    material.roughness = Kern::LoadTexture("assets/textures/sub_metallic.png");
    material.metallic = Kern::LoadTexture("assets/textures/sub_metallic.png");
    material.normal = Kern::LoadTexture("assets/textures/sub_normal.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "default",
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
        );
    loadModel("assets/models/uboot.obj");

    spotlightLeft = LightManager::getInstance().createSpotLight(
        glm::vec3(1.0f, 0.95f, 0.8f), 100.0f, 12.5f, 17.5f
    );

    spotlightRight = LightManager::getInstance().createSpotLight(
        glm::vec3(1.0f, 0.95f, 0.8f), 100.0f, 12.5f, 17.5f
    );

    spotlightTimer = new Timer();
    addChild(spotlightTimer);
    spotlightTimer->startTimer(10.0f, [this]() {
        this->onTimerEnd();
    });

    for (int i = 0; i < 4; i++) {
        auto * rotor = new UbootRotor();
        addChild(rotor);

        auto * emitter = new ParticleEmitter(5000);
        addChild(emitter);
        const auto newPos = glm::vec3(-0.28f * (i < 2 ? 1.0f : -1.0f), 0.38f * (i == 1 || i == 3 ? -1.0f : 1.0f), 0.8f);
        rotor->transform.position = newPos;
        emitter->transform.position = newPos;
        rotors[i] = rotor;
        emitters[i] = emitter;
    }
}

void Uboot::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform){
    targetMoveSpeed = 0.0f;
    targetRotorSpeed = 0.0f;
    targetRollVelocity = 0.0f;

    processInput(window, deltaTime);

    constexpr float accelerationSpeed = 0.75f;
    const float tSpeed = 1.0f - glm::exp(-accelerationSpeed * deltaTime);

    actualMoveSpeed = glm::mix(actualMoveSpeed, targetMoveSpeed, tSpeed);
    actualRotorSpeed = glm::mix(actualRotorSpeed, targetRotorSpeed, tSpeed * 2);

    for (UbootRotor * rotor : rotors) {
        rotor->rotorSpeed = actualRotorSpeed;
    }

    handleRolls(window, deltaTime);

    const auto time = static_cast<float>(glfwGetTime());
    const float waveHeight = getWaterHeight(transform.position.x, transform.position.z, time);
    constexpr float floatOffset = 0.1f;

    if (const float maxSurfaceY = waveHeight + floatOffset; transform.position.y >= maxSurfaceY - 0.15f) {
        transform.position.y = maxSurfaceY;

        if (pitchVelocity > 0.0f) {
            pitchVelocity = 0.0f;
        }
    }

    updateCameraTransform(cameraTransform, deltaTime);

    for (ParticleEmitter * emitter : emitters) {
        emitter->active = (transform.position.y < waveHeight) && (glm::abs(actualRotorSpeed) > 2.0f);
    }

    const glm::vec3 right = glm::cross(transform.forward(), transform.up());
    if (spotlightLeft != nullptr && spotlightRight != nullptr) {
        spotlightLeft->position = transform.position + transform.forward() * 1.0f - right * 0.4f + transform.up() * 0.75f;
        spotlightLeft->direction = transform.forward();

        spotlightRight->position = transform.position + transform.forward() * 1.0f + right * 0.4f + transform.up() * 0.75f;
        spotlightRight->direction = transform.forward();
    }
}

void Uboot::prepareUniforms() const {
    if (const GLint timeLocation = glGetUniformLocation(material.shader, "time"); timeLocation != -1) {
        glUniform1f(timeLocation, static_cast<float>(glfwGetTime()));
    }

    if (const GLint deformationLocation = glGetUniformLocation(material.shader, "deformationThreshold"); deformationLocation != -1) {
        glUniform1f(deformationLocation, static_cast<float>(2));
    }
}

void Uboot::processInput(GLFWwindow* window, const float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        targetMoveSpeed = moveSpeed;
        targetRotorSpeed = rotorSpeed;
        transform.position += transform.forward() * actualMoveSpeed * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        targetMoveSpeed = moveSpeedBackward;
        targetRotorSpeed = rotorSpeedBackward;
        transform.position -= transform.forward() * actualMoveSpeed * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        targetMoveSpeed = moveSpeedBackward;
        targetRotorSpeed = rotorSpeedBackward;
        targetRollVelocity = -angleSpeed;
        transform.position -= transform.right() * actualMoveSpeed / 2.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        targetMoveSpeed = moveSpeedBackward;
        targetRotorSpeed = rotorSpeedBackward;
        targetRollVelocity = angleSpeed;
        transform.position += transform.right() * actualMoveSpeed / 2.0f * deltaTime;
    }
}

void Uboot::updateCameraTransform(Transform& cameraTransform, float deltaTime) const {
    const glm::vec3 shipPos = transform.position;
    const glm::vec3 forward = transform.forward();
    const glm::vec3 up = transform.up();

    const glm::vec3 targetCamPos = shipPos - forward * 3.0f + up * 0.5f;
    glm::vec3 swayedCamPos = targetCamPos;

    if (transform.position.y < getWaterHeight(transform.position.x, transform.position.z, deltaTime)) {
        auto t = static_cast<float>(glfwGetTime());
        float swayX = sin(t * 1.2f) * 0.15f;
        float swayY = cos(t * 1.0f) * 0.10f;
        float swayZ = sin(t * 0.8f) * 0.15f;
        swayedCamPos += glm::vec3(swayX, swayY, swayZ);
    }

    const glm::quat currentCamRot = cameraTransform.rotation;
    cameraTransform.lookAt(shipPos + up * 0.5f, up);
    const glm::quat targetCamRot = cameraTransform.rotation;

    constexpr float camFollowSpeed = 6.0f;
    constexpr float camRotateSpeed = 8.0f;

    const float tFollow = 1.0f - glm::exp(-camFollowSpeed * deltaTime);
    const float tRotate = 1.0f - glm::exp(-camRotateSpeed * deltaTime);

    cameraTransform.position = glm::mix(cameraTransform.position, swayedCamPos, tFollow);
    cameraTransform.rotation = glm::slerp(currentCamRot, targetCamRot, tRotate);
}

void Uboot::handleRolls(GLFWwindow* window, const float deltaTime) {
    const glm::vec3 right = glm::cross(transform.forward(), transform.up());
    const float rollError = glm::dot(right, glm::vec3(0.0f, 1.0f, 0.0f));
    constexpr float stabilizationSpeed = 6.0f;
    targetRollVelocity += rollError * stabilizationSpeed;

    const float tRoll = 1.0f - glm::exp(-10.0f * deltaTime);
    rollVelocity = glm::mix(rollVelocity, targetRollVelocity, tRoll);
    transform.roll(rollVelocity * deltaTime);

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse) {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    double xoffset = mouseX - lastX;
    double yoffset = lastY - mouseY;

    lastX = mouseX;
    lastY = mouseY;

    constexpr float mouseSensitivity = 0.15f;
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    const auto targetPitchVelocity = static_cast<float>(yoffset);
    const auto targetYawVelocity = -static_cast<float>(xoffset);

    constexpr float shipInertia = 8.0f;
    pitchVelocity = glm::mix(pitchVelocity, targetPitchVelocity, shipInertia * deltaTime);
    yawVelocity = glm::mix(yawVelocity, targetYawVelocity, shipInertia * deltaTime);

    transform.pitch(pitchVelocity * deltaTime);
    transform.yaw(yawVelocity * deltaTime);
}

void Uboot::onTimerEnd() {
    spotlightLeft->intensity = spotlightLeft->intensity == 100.0f ? 0.0f: 100.0f;
    spotlightRight->intensity = spotlightRight->intensity == 100.0f ? 0.0f: 100.0f;
    spotlightTimer->startTimer(0.5f, [this]() {
        this->onTimerEnd();
    });
}

float Uboot::getWaterHeight(const float x, const float z, const float t) {
    CPUWave waves[3] = {
        { glm::normalize(glm::vec2(1.0f, 0.1f)),  0.25f, 12.0f, 1.5f },
        { glm::normalize(glm::vec2(0.2f, 1.0f)),  0.15f, 6.0f,  1.0f },
        { glm::normalize(glm::vec2(-0.5f, 0.5f)), 0.08f, 3.0f,  0.8f }
    };

    float y = 0.0f;
    for (auto & [direction, amplitude, wavelength, speed] : waves) {
        const float k = 2.0f * 3.14159265f / wavelength;
        const float c = glm::sqrt(9.81f / k) * speed;
        const float f = k * (glm::dot(direction, glm::vec2(x, z)) - c * t);
        y += amplitude * glm::sin(f);
    }
    return y;
}
