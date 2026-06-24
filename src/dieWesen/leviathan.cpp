#include "leviathan.h"
#include "uboot.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/groupManager.h"
#include "werkzeuge/visual/questManager.h"
#include <gtx/quaternion.hpp>

extern Kamera kamera;

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

    boundingRadius = 8.0f;

    music = dynamic_cast<UnterwasserszeneAudioHelper * >(getNodesInGroup("Music")[0]);

    basePosition = transform.position;
    patrolCenter = transform.position;
}

Wesen* Leviathan::findClosestTarget() const {
    Wesen* closest = nullptr;
    float closestDist = std::numeric_limits<float>::max();

    const auto& subs = getNodesInGroup("player");
    for (auto* sub : subs) {
        auto* uboot = dynamic_cast<Uboot*>(sub);
        if (uboot == nullptr) continue;
        float d = glm::distance(basePosition, uboot->getGlobalTransform().position);
        if (d < closestDist) {
            closestDist = d;
            closest = uboot;
        }
    }

    const auto& walkers = getNodesInGroup("playerWalking");
    for (auto* walker : walkers) {
        float d = glm::distance(basePosition, walker->getGlobalTransform().position);
        if (d < closestDist) {
            closestDist = d;
            closest = walker;
        }
    }

    return closest;
}

void Leviathan::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    elapsedTime += deltaTime;

    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
    }

    Wesen* target = findClosestTarget();
    if (target == nullptr) {
        patrol(deltaTime);
        return;
    }

    glm::vec3 targetPos = target->getGlobalTransform().position;
    glm::vec3 toTargetWorld = targetPos - basePosition;
    float distance = glm::length(toTargetWorld);

    if (distance < 300.0f) {
        chase(deltaTime, toTargetWorld, distance);

        float actualDist = glm::distance(transform.position, targetPos);
        float hitDist = boundingRadius + target->boundingRadius;
        if (actualDist < hitDist && attackCooldown <= 0.0f) {
            if (auto* uboot = dynamic_cast<Uboot*>(target)) {
                uboot->schadenNehmen(attackDamage);
                attackCooldown = attackInterval;
            }
        }

        if (distance < 150.0f && !wasChasing) {
            QuestManager::getInstance().progressObjective("leviathan_encounter");
        }
        wasChasing = isChasing;
    } else {
        if (wasChasing) {
            wasChasing = false;
        }
        music->stopChasing();
        patrol(deltaTime);
    }
}

void Leviathan::chase(float deltaTime, const glm::vec3& toTarget, float distance) {
    music->initiateChase();

    glm::vec3 targetDir = glm::normalize(toTarget);
    glm::quat baseOrientation = Transform::quatLookAt(-targetDir, glm::vec3(0.0f, 1.0f, 0.0f));

    float bodyTurnSpeed = 1.15f;
    glm::quat slerpedRotation = glm::slerp(transform.rotation, baseOrientation, glm::clamp(bodyTurnSpeed * deltaTime, 0.0f, 1.0f));
    slerpedRotation = glm::normalize(slerpedRotation);

    float angle = u_spiralFrequency * elapsedTime;
    float rollAngle = glm::sin(angle) * maxRollOffset;

    glm::quat rollOffset = glm::angleAxis(glm::radians(rollAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    transform.rotation = slerpedRotation * rollOffset;

    glm::mat4 invRot = glm::toMat4(glm::conjugate(transform.rotation));
    glm::vec3 localToTarget = glm::normalize(glm::vec3(invRot * glm::vec4(toTarget, 0.0f)));

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
}

void Leviathan::patrol(float deltaTime) {
    float angle = elapsedTime * 0.3f;
    float patrolRadius = 30.0f;

    glm::vec3 patrolTarget = patrolCenter + glm::vec3(
        glm::cos(angle) * patrolRadius,
        glm::sin(angle * 0.5f) * 5.0f,
        glm::sin(angle) * patrolRadius
    );

    glm::vec3 toPatrol = patrolTarget - basePosition;
    float dist = glm::length(toPatrol);
    if (dist > 0.5f) {
        glm::vec3 dir = glm::normalize(toPatrol);
        glm::quat targetRot = Transform::quatLookAt(-dir, glm::vec3(0.0f, 1.0f, 0.0f));
        transform.rotation = glm::slerp(transform.rotation, targetRot, glm::clamp(0.8f * deltaTime, 0.0f, 1.0f));
        transform.rotation = glm::normalize(transform.rotation);

        basePosition -= transform.forward() * idleSpeed * deltaTime;
    }

    float spiralAngle = u_spiralFrequency * elapsedTime;
    glm::vec3 offset = (transform.right() * glm::cos(spiralAngle) + transform.up() * glm::sin(spiralAngle)) * u_spiralRadius;
    transform.position = basePosition + offset;

    u_neckPitch = glm::mix(u_neckPitch, 0.0f, glm::clamp(2.0f * deltaTime, 0.0f, 1.0f));
    u_neckYaw = glm::mix(u_neckYaw, 0.0f, glm::clamp(2.0f * deltaTime, 0.0f, 1.0f));
    u_emissionPulse = 1.0f + glm::sin(elapsedTime * 1.5f) * 0.2f;
}

void Leviathan::prepareUniforms() const {
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
