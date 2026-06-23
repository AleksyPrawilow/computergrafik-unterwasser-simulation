//
// Created by Alexey Pravilov on 20/06/2026.
//

#include "bottle.h"

#include "uboot.h"
#include "unterwasserszeneQuests.h"
#include "werkzeuge/input.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/visual/tween.h"

extern Kamera kamera;

void Bottle::init() {
    name = "Bottle";

    material.albedo = Kern::LoadTexture("assets/textures/bottle_albedo.png");
    material.normal = Kern::LoadTexture("assets/textures/bottle_normal.png");
    material.roughness = Kern::LoadTexture("assets/textures/bottle_roughness.png");
    material.metallic = Kern::LoadTexture("assets/textures/bottle_roughness.png");
    material.opacity = Kern::LoadTexture("assets/textures/jellyfish_opacity.png");
    material.shader = ShaderManager::getInstance().getShader("default");
    material.isTransparent = true;
    loadModel("assets/models/bottle.obj");

    cork = new Wesen();
    cork->material.albedo = Kern::LoadTexture("assets/textures/bottle_albedo.png");
    cork->material.normal = Kern::LoadTexture("assets/textures/bottle_normal.png");
    cork->material.roughness = Kern::LoadTexture("assets/textures/bottle_roughness.png");
    cork->material.metallic = Kern::LoadTexture("assets/textures/bottle_roughness.png");
    cork->material.shader = ShaderManager::getInstance().getShader("default");
    cork->loadModel("assets/models/bottle_cork.obj");
    addChild(cork);
    beginSwim();

    interactLabel = new UILabel();
    interactLabel->text = "Press 'E' to pick up";
    interactLabel->fontSize = 48.0f;
    interactLabel->expansion = UIExpansion::CENTER;
    parent->addChild(interactLabel);
    glm::vec2 screenSize = Kern::GetViewportSize();
    interactLabel->transform.position = glm::vec3(screenSize.x / 2.0f, screenSize.y / 2.0f, 0.0f);
    interactLabel->visible = false;

    uiMarker = new WorldspaceUI();
    parent->addChild(uiMarker);
    uiMarker->material.albedo = Kern::LoadTexture("assets/textures/quest_marker.png");
    uiMarker->transform.scale = glm::vec3(128.0f, 128.0f, 1.0f);
    uiMarker->expansion = UIExpansion::CENTER;
    uiMarker->setTarget(this, glm::vec3(0.0f, 4.0f, 0.0f));
    uiMarker->shouldScale = true;
    uiMarker->enabled = false;

    map = new Map();
    map->mapEuler.x = 90.0f;
    addChild(map);

    isCollidable = true;
    player = dynamic_cast<Player *>(getNodesInGroup("playerWalking")[0]);

    addToGroup("bottle");
}

void Bottle::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    if (!isPicked) {
        const auto time = static_cast<float>(glfwGetTime());
        glm::vec3 pos = transform.position;

        float midY = (localAABB.min.y + localAABB.max.y) * 0.5f;

        glm::vec3 corners[4] = {
            glm::vec3(localAABB.min.x, midY, localAABB.min.z),
            glm::vec3(localAABB.max.x, midY, localAABB.min.z),
            glm::vec3(localAABB.max.x, midY, localAABB.max.z),
            glm::vec3(localAABB.min.x, midY, localAABB.max.z)
        };

        float heights[4];
        glm::vec3 points[4];

        glm::mat4 model = getGlobalModelMatrix();
        for (int i = 0; i < 4; i++) {
            points[i] = glm::vec3(model * glm::vec4(corners[i], 1.0f));
            heights[i] = Uboot::getWaterHeight(points[i].x, points[i].z, time);
        }

        const float avgHeight = (heights[0] + heights[1] + heights[2] + heights[3]) * 0.5f;

        glm::vec3 targetPos = pos;
        targetPos.y = avgHeight - 1.0f;

        transform.position = glm::mix(
            transform.position,
            targetPos,
            1.0f - exp(-heightSmooth * deltaTime)
        );

        float front = (heights[2] + heights[3]) * 1.5f;
        float back  = (heights[0] + heights[1]) * 1.5f;
        float right = (heights[1] + heights[2]) * 1.5f;
        float left  = (heights[0] + heights[3]) * 1.5f;

        float pitch = (back - front) * tiltStrength;
        float roll  = (left - right) * tiltStrength;

        glm::quat targetRot = glm::quat(glm::vec3(pitch, 0.0f, roll));

        transform.rotation = glm::slerp(
            transform.rotation,
            targetRot,
            1.0f - glm::exp(-rotSmooth * deltaTime)
        );
    } else {
        transform.rotation = glm::quat(glm::radians(euler));
    }

    if (canBePicked) {
        if (player->raycast->isColliding() && player->raycast->getCollider() == this) {
            interactLabel->visible = true;
            if (Input::isKeyJustPressed(GLFW_KEY_E)) {
                canBePicked = false;
                pickup();
                interactLabel->visible = false;
                uiMarker->visible = false;
            }
        } else {
            interactLabel->visible = false;
        }
    }
}

void Bottle::beginSwim() {
    auto targetIslandPos = glm::vec3(-669.0, 0.0f, -199.0);
    createTween()
        ->tweenProperty(&transform.position.x, targetIslandPos.x, 5.0f, EaseType::EASE_OUT_SINE)
        ->parallel()
        ->tweenProperty(&transform.position.z, targetIslandPos.z, 5.0f, EaseType::EASE_OUT_SINE)
        ->tweenCallback([this]() {
            canBePicked = true;
            uiMarker->enabled = true;
            dynamic_cast<UnterwasserszeneQuests *>(getNodesInGroup("QuestManager")[0])->setQuest(UnterwasserszeneQuests::COLLECT_BOTTLE);
        });
}


void Bottle::pickup() {
    isPicked = true;
    map->unwrap(2.0f);
    createTween()
        ->tweenProperty(&cork->transform.position.y, 0.5f, 1.0f, EaseType::EASE_OUT_BACK)
        ->parallel()
        ->tweenProperty(&euler, glm::vec3(0.0f), 1.0f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&cork->transform.position.z, -0.5f, 1.0f, EaseType::EASE_OUT_SINE)
        ->tweenProperty(&map->transform.position.y, 4.0f, 1.0f, EaseType::EASE_OUT_BACK);
}
