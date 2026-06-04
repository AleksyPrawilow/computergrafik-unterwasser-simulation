//
// Created by Alexey Pravilov on 02/06/2026.
//
#include "uboot.h"
#include "../werkzeuge/textur.h"
#include "werkzeuge/shaderManager.h"

void Uboot::init() {
    material.albedo = Kern::LoadTexture("assets/textures/albedo.png");
    material.roughness = Kern::LoadTexture("assets/textures/roughness.png");
    material.metallic = Kern::LoadTexture("assets/textures/metallness.png");
    material.normal = Kern::LoadTexture("assets/textures/normal.png");
    material.shader = ShaderManager::getInstance().loadShader(
        "uboot",
        "assets/shaders/shader_5_1_ship.vert",
        "assets/shaders/shader_5_1_ship.frag"
        );
    loadModel("assets/models/11097_squid_v1.obj");
}

void Uboot::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform){
    float angleSpeed = 4.0f;
    float moveSpeed = 18.0f;
    float moveSpeedBackward = 12.0f;
    float targetMoveSpeed = 0.0f;
    float targetRollVelocity = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        targetMoveSpeed = moveSpeed;
        transform.position += transform.forward() * actualMoveSpeed * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        targetMoveSpeed = moveSpeedBackward;
        transform.position -= transform.forward() * actualMoveSpeed * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) targetRollVelocity = -angleSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) targetRollVelocity = angleSpeed;

    constexpr float accelerationSpeed = 0.75f;
    float tSpeed = 1.0f - glm::exp(-accelerationSpeed * deltaTime);
    actualMoveSpeed = glm::mix(actualMoveSpeed, targetMoveSpeed, tSpeed);

    glm::vec3 right = glm::cross(transform.forward(), transform.up());

    // Calculate the tilt error relative to world up (0, 1, 0)
    float rollError = glm::dot(right, glm::vec3(0.0f, 1.0f, 0.0f));

    // How fast the squid fights rotation and snaps back upright
    constexpr float stabilizationSpeed = 6.0f;

    // Apply the restorative torque
    targetRollVelocity += rollError * stabilizationSpeed;
    // --------------------------------------------------

    // 3. Apply the rolling transformation (keeps your existing mix code)
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


    const glm::vec3 shipPos = transform.position;
    const glm::vec3 forward = transform.forward();
    const glm::vec3 up = transform.up();

    const glm::vec3 targetCamPos = shipPos - forward * 3.0f + up * 0.5f;

    const glm::quat currentCamRot = cameraTransform.rotation;
    cameraTransform.lookAt(shipPos + up * 0.5f, up);
    const glm::quat targetCamRot = cameraTransform.rotation;

    constexpr float camFollowSpeed = 6.0f;
    constexpr float camRotateSpeed = 8.0f;

    const float tFollow = 1.0f - glm::exp(-camFollowSpeed * deltaTime);
    const float tRotate = 1.0f - glm::exp(-camRotateSpeed * deltaTime);

    cameraTransform.position = glm::mix(cameraTransform.position, targetCamPos, tFollow);
    cameraTransform.rotation = glm::slerp(currentCamRot, targetCamRot, tRotate);
}

void Uboot::prepareUniforms() const {
    if (const GLint timeLocation = glGetUniformLocation(material.shader, "time"); timeLocation != -1) {
        glUniform1f(timeLocation, static_cast<float>(glfwGetTime()));
    }
}
