#include "leviathan.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/groupManager.h"
#include <gtx/quaternion.hpp>
#include <iostream>

void Leviathan::init() {
    name = "leviathan";
    
    loadModel("assets/models/leviathan.obj");
    material.albedo = Kern::LoadTexture("assets/textures/leviathan_albedo.png");
    material.normal = Kern::LoadTexture("assets/textures/leviathan_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/leviathan_metallic.png");
    material.roughness = Kern::LoadTexture("assets/textures/leviathan_metallic.png");
    material.emission = Kern::LoadTexture("assets/textures/leviathan_emissive.png");
    material.bloomStrength = 0.02f;
    
    material.shader = ShaderManager::getInstance().loadShader(
        "leviathan",
        "assets/shaders/leviathan.vert",
        "assets/shaders/default.frag"
    );
    
    addToGroup("feinde");
    addToGroup("Leviathan");

    music = dynamic_cast<UnterwasserszeneAudioHelper * >(getNodesInGroup("Music")[0]);

    basePosition = transform.position;
}

void Leviathan::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    Wesen* target = nullptr;
    const auto& players = getNodesInGroup("player");
    if (!players.empty()) {
        target = players[0];
    } else {
        const auto& walkers = getNodesInGroup("playerWalking");
        if (!walkers.empty()) {
            target = walkers[0];
        }
    }

    if (target == nullptr) return;

    elapsedTime += deltaTime;

    glm::vec3 targetPos = target->getGlobalTransform().position;
    glm::vec3 toTargetWorld = targetPos - basePosition;
    float distance = glm::length(toTargetWorld);

    if (distance > 2.0f && distance < 300.0f) {
        music->initiateChase();
        glm::vec3 targetDir = glm::normalize(toTargetWorld);
        glm::quat baseOrientation = Transform::quatLookAt(-targetDir, glm::vec3(0.0f, 1.0f, 0.0f));

        float bodyTurnSpeed = 1.15f;
        glm::quat slerpedRotation = glm::slerp(transform.rotation, baseOrientation, glm::clamp(bodyTurnSpeed * deltaTime, 0.0f, 1.0f));
        slerpedRotation = glm::normalize(slerpedRotation);

        float angle = u_spiralFrequency * elapsedTime;
        float rollAngle = glm::sin(angle) * maxRollOffset;

        glm::quat rollOffset = glm::angleAxis(glm::radians(rollAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        transform.rotation = slerpedRotation * rollOffset;

        glm::mat4 invRot = glm::toMat4(glm::conjugate(transform.rotation));
        glm::vec3 localToTarget = glm::normalize(glm::vec3(invRot * glm::vec4(toTargetWorld, 0.0f)));

        float targetYaw = glm::atan(-localToTarget.x, localToTarget.z);
        float targetPitch = glm::asin(localToTarget.y);

        float neckTurnSpeed = 4.5f;
        u_neckPitch = glm::mix(u_neckPitch, targetPitch, glm::clamp(neckTurnSpeed * deltaTime, 0.0f, 1.0f));
        u_neckYaw = glm::mix(u_neckYaw, targetYaw, glm::clamp(neckTurnSpeed * deltaTime, 0.0f, 1.0f));

        isChasing = (distance < 80.0f);
        float pulseSpeed = isChasing ? 8.0f : 2.5f;
        u_emissionPulse = 1.0f + glm::sin(elapsedTime * pulseSpeed) * 0.45f;

        float turnAlignmentFactor = glm::max(glm::cos(u_neckYaw) * glm::cos(u_neckPitch), 0.15f);
        float activeMoveSpeed = moveSpeed * turnAlignmentFactor;

        basePosition -= transform.forward() * activeMoveSpeed * deltaTime;
        glm::vec3 offset = (transform.right() * glm::cos(angle) + transform.up() * glm::sin(angle)) * u_spiralRadius;

        transform.position = basePosition + offset;
    } else {
        music->stopChasing();
    }
}

void Leviathan::prepareUniforms() const {
    // Pass everything to the shader pipeline
    Kern::setUniform(material.shader, "time",             static_cast<float>(glfwGetTime()));
    Kern::setUniform(material.shader, "u_swimSpeed",       u_swimSpeed);
    Kern::setUniform(material.shader, "u_swimAmplitude",   u_swimAmplitude);
    Kern::setUniform(material.shader, "u_neckPitch",       u_neckPitch);
    Kern::setUniform(material.shader, "u_neckYaw",         u_neckYaw);
    Kern::setUniform(material.shader, "u_neckPivotZ",      u_neckPivotZ);
    Kern::setUniform(material.shader, "u_swimFrequency",   u_swimFrequency);
    Kern::setUniform(material.shader, "u_neckLength",      u_neckLength);
    Kern::setUniform(material.shader, "u_emissionPulse",   u_emissionPulse);
}