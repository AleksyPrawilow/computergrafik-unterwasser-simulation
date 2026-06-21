//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "tree.h"

#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/questManager.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void Tree::init() {
    name = "tree";
    isCollidable = true;
    loadModel("assets/models/tree.obj");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.albedo = Kern::LoadTexture("assets/textures/Raft_baseColor.png");
    material.normal = Kern::LoadTexture("assets/textures/Raft_normal.png");
    material.metallic = Kern::LoadTexture("assets/textures/Raft_metallicRoughness.png");
    material.roughness = Kern::LoadTexture("assets/textures/Raft_metallicRoughness.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    transform.position = glm::vec3(0.0f, 6.0f, 0.0f);
    transform.scale = glm::vec3(1.0f);
    fallSound = new AudioPlayer("assets/audio/tree_fall.mp3", false, 10);
    addChild(fallSound);
}

void Tree::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    transform.rotation = glm::quat(glm::radians(treeEuler));
}

void Tree::hit(const glm::vec3 hitDir) {
    if (hp <= 0) {
        return;
    }

    const auto originalPos = glm::vec3(transform.position);
    constexpr float displacement = 0.15f;
    hp -= 1;

    createTween()
        ->tweenProperty(&transform.position.x, transform.position.x + hitDir.x * displacement, 0.05f, EaseType::EASE_OUT_CUBIC)
        ->parallel()
        ->tweenProperty(&transform.position.z, transform.position.z + hitDir.z * displacement, 0.05f, EaseType::EASE_OUT_CUBIC)
        ->tweenProperty(&transform.position.x, originalPos.x, 0.15f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&transform.position.z, originalPos.z, 0.15f, EaseType::EASE_OUT_BACK)
        ->tweenCallback([this, hitDir]() {
            if (hp <= 0) {
                this->fall(hitDir);
            }
        });
}

void Tree::fall(glm::vec3 hitDir) {
    fallSound->play();
    createTween()
        ->tweenProperty(&treeEuler.z, 3.0f, 0.15f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&treeEuler.z, 8.0f, 0.25f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&treeEuler.x, -3.0f, 0.25f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&treeEuler.z, 90.0f, 2.20f, EaseType::EASE_IN_CUBIC)
        ->parallel()
        ->tweenProperty(&treeEuler.x, -35.0f, 2.20f, EaseType::EASE_IN_CUBIC)
        ->tweenCallback([this]() {
            kamera.addShake(0.25f, 0.4f);
            QuestManager::getInstance().progressObjective("chop_tree", 1);

            Quest craftingQuest;
            craftingQuest.title = "Crafty time";

            QuestObjective digObjective;
            digObjective.tag = "craft_shovel";
            digObjective.description = "Craft a shovel";
            digObjective.requiredCount = 1;

            craftingQuest.objectives.push_back(digObjective);

            craftingQuest.onComplete = []() {
                std::cout << "QUEST COMPLETED: You found the sunken treasure!" << std::endl;
                AudioManager::getInstance().play2D("assets/audio/quest_complete.wav", false, true);
            };

            QuestManager::getInstance().acceptQuest(craftingQuest);
        })
        ->tweenProperty(&treeEuler.z, 88.0f, 0.40f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&treeEuler.x, -33.0f, 0.40f, EaseType::EASE_OUT_BACK);
}
