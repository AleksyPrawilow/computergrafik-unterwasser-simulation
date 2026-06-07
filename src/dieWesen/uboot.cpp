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
    float angleSpeed = 2.0f;
    float moveSpeed = 10.0f;
    float moveSpeedBackward = 5.0f;
    float targetMoveSpeed = 0.0f;
    float targetRollVelocity = 0.0f;
    float targetRotorSpeed = 0.0f;
    float rotorSpeed = 6.0f;
    float rotorSpeedBackward = -2.5f;

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

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) targetRollVelocity = -angleSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) targetRollVelocity = angleSpeed;

    constexpr float accelerationSpeed = 0.75f;
    float tSpeed = 1.0f - glm::exp(-accelerationSpeed * deltaTime);

    actualMoveSpeed = glm::mix(actualMoveSpeed, targetMoveSpeed, tSpeed);
    actualRotorSpeed = glm::mix(actualRotorSpeed, targetRotorSpeed, tSpeed * 2);

    for (UbootRotor * rotor : rotors) {
        rotor->rotorSpeed = actualRotorSpeed;
    }

    glm::vec3 right = glm::cross(transform.forward(), transform.up());
    float rollError = glm::dot(right, glm::vec3(0.0f, 1.0f, 0.0f));
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

    const auto time = static_cast<float>(glfwGetTime());
    float waveHeight = getWaterHeight(transform.position.x, transform.position.z, time);

    float floatOffset = 0.1f;
    float maxSurfaceY = waveHeight + floatOffset;

    if (transform.position.y >= maxSurfaceY - 0.15f) {
        transform.position.y = maxSurfaceY;

        if (pitchVelocity > 0.0f) {
            pitchVelocity = 0.0f;
        }
    }

    const glm::vec3 shipPos = transform.position;
    const glm::vec3 forward = transform.forward();
    const glm::vec3 up = transform.up();

    const glm::vec3 targetCamPos = shipPos - forward * 3.0f + up * 0.5f;

    // 2. --- ADD THIS: WATER CURRENT SWAY ---
    glm::vec3 swayedCamPos = targetCamPos;

    // Only sway the camera if the submarine is submerged
    if (transform.position.y < waveHeight) {
        auto t = static_cast<float>(glfwGetTime());

        // We use slightly different speeds (1.2, 1.0, 0.8) and amplitudes
        // so the sway looks like an irregular, organic water current rather than a perfect circle.
        float swayX = sin(t * 1.2f) * 0.15f; // Horizontal drift (15cm)
        float swayY = cos(t * 1.0f) * 0.10f; // Vertical bobbing (10cm)
        float swayZ = sin(t * 0.8f) * 0.15f; // Forward/Backward drift (15cm)

        swayedCamPos += glm::vec3(swayX, swayY, swayZ);
    }
    // ---------------------------------------

    const glm::quat currentCamRot = cameraTransform.rotation;
    cameraTransform.lookAt(shipPos + up * 0.5f, up);
    const glm::quat targetCamRot = cameraTransform.rotation;

    constexpr float camFollowSpeed = 6.0f;
    constexpr float camRotateSpeed = 8.0f;

    const float tFollow = 1.0f - glm::exp(-camFollowSpeed * deltaTime);
    const float tRotate = 1.0f - glm::exp(-camRotateSpeed * deltaTime);

    cameraTransform.position = glm::mix(cameraTransform.position, swayedCamPos, tFollow);
    cameraTransform.rotation = glm::slerp(currentCamRot, targetCamRot, tRotate);

    for (ParticleEmitter * emitter : emitters) {
        emitter->active = (transform.position.y < waveHeight) && (glm::abs(actualRotorSpeed) > 2.0f);
    }

    if (spotlightLeft != nullptr && spotlightRight != nullptr) {
        spotlightLeft->position = transform.position + forward * 1.0f - right * 0.4f + up * 0.75f;
        spotlightLeft->direction = forward;

        spotlightRight->position = transform.position + forward * 1.0f + right * 0.4f + up * 0.75f;
        spotlightRight->direction = forward;
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
