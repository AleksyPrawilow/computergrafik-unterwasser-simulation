//
// Created by Alexey Pravilov on 02/06/2026.
//
#include "uboot.h"
#include "../werkzeuge/textur.h"

void Uboot::init() {
    material.albedo = Core::LoadTexture("assets/textures/albedo.png");
    material.roughness = Core::LoadTexture("assets/textures/roughness.png");
    material.metallic = Core::LoadTexture("assets/textures/metallness.png");
    material.normal = Core::LoadTexture("assets/textures/normal.png");
    loadModel("assets/models/spaceship2.obj");
}

void Uboot::update(GLFWwindow* window, float deltaTime, Transform& cameraTransform){
    float angleSpeed = 0.02f;
    float moveSpeed = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        transform.position += transform.forward() * moveSpeed;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        transform.position -= transform.forward() * moveSpeed;

    float targetYawVelocity = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) targetYawVelocity = angleSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) targetYawVelocity = -angleSpeed;

    yawVelocity = glm::mix(yawVelocity, targetYawVelocity, 10.0f * deltaTime);
    transform.yaw(yawVelocity);

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse)
    {
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
    const auto targetRollVelocity = -static_cast<float>(xoffset);

    constexpr float shipInertia = 8.0f;
    pitchVelocity = glm::mix(pitchVelocity, targetPitchVelocity, shipInertia * deltaTime);
    rollVelocity = glm::mix(rollVelocity, targetRollVelocity, shipInertia * deltaTime);

    transform.pitch(pitchVelocity * deltaTime);
    transform.roll(rollVelocity * deltaTime);


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
