#include "fishBolid.h"
#include "werkzeuge/groupManager.h"
#include "gtc/matrix_transform.hpp"
#include "flockManager.h"
#include "path3d.h"

void FishBoid::init() {
    addToGroup("Boids");

    velocity = glm::vec3(
        (rand() % 100 / 50.0f) - 1.0f,
        (rand() % 100 / 50.0f) - 1.0f,
        (rand() % 100 / 50.0f) - 1.0f
    );
    if (glm::length(velocity) > 0.001f) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }
}

void FishBoid::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {
    acceleration = glm::vec3(0.0f);
    const auto& flock = getNodesInGroup("Boids");

    glm::vec3 sep = calculateSeparation(flock);
    glm::vec3 ali = calculateAlignment(flock);
    glm::vec3 coh = calculateCohesion(flock);

    glm::vec3 surfAvo = calculateSurfaceAvoidance();
    glm::vec3 playerForce = calculatePlayerInteraction(cameraTransform);

    glm::vec3 pathForce = calculatePathFollowing();

    acceleration += sep * separationWeight;
    acceleration += ali * alignmentWeight;
    acceleration += coh * cohesionWeight;
    acceleration += surfAvo * surfaceAvoidanceWeight;
    acceleration += playerForce;
    acceleration += pathForce * pathFollowingWeight;

    velocity += acceleration * deltaTime;
    velocity = limit(velocity, maxSpeed);
    transform.position += velocity * deltaTime;

    if (glm::length(velocity) > 0.01f) {
        transform.lookAt(transform.position + velocity);

        if (glm::abs(rotationOffsetY) > 0.001f) {
            transform.rotation = transform.rotation * glm::angleAxis(glm::radians(rotationOffsetY), glm::vec3(0, 1, 0));
        }
    }
}

glm::vec3 FishBoid::calculatePathFollowing() {
    glm::vec3 steer(0.0f);
    glm::vec3 globalPos = getGlobalTransform().position;

    FishFlock* flockManager = dynamic_cast<FishFlock*>(parent);

    if (!flockManager || !flockManager->targetPath || flockManager->targetPath->frames.empty()) return steer;

    const auto& path = flockManager->targetPath->frames;

    float minDist = 999999.0f;
    int closestIndex = 0;

    for (int i = 0; i < path.size(); i++) {
        float d = glm::distance(globalPos, path[i].position);
        if (d < minDist) {
            minDist = d;
            closestIndex = i;
        }
    }

    int lookAheadSteps = 5;
    int targetIndex = (closestIndex + lookAheadSteps) % path.size();
    const PathFrame& targetFrame = path[targetIndex];

    glm::vec3 desiredVelocity = targetFrame.position - globalPos;
    if (glm::length(desiredVelocity) > 0.001f) {
        desiredVelocity = glm::normalize(desiredVelocity) * maxSpeed;
        steer = limit(desiredVelocity - velocity, maxForce);
    }

    return steer;
}

glm::vec3 FishBoid::calculateSurfaceAvoidance() {
    glm::vec3 steer(0.0f);
    glm::vec3 globalPos = getGlobalTransform().position;

    float distanceToSurface = waterLevel - globalPos.y;

    if (distanceToSurface < 3.0f && distanceToSurface > -5.0f) {
        glm::vec3 desired = glm::vec3(velocity.x, -maxSpeed, velocity.z);
        if (glm::length(desired) > 0.001f) {
            desired = glm::normalize(desired) * maxSpeed;
            glm::vec3 steerDown = desired - velocity;
            float strength = glm::clamp(1.0f - (distanceToSurface / 3.0f), 0.0f, 1.0f);
            steer += limit(steerDown, maxForce * 2.5f) * strength;
        }
    }

    float oceanFloorLevel = -40.0f;
    float distanceToFloor = globalPos.y - oceanFloorLevel;
    if (distanceToFloor < 3.0f) {
         glm::vec3 desired = glm::vec3(velocity.x, maxSpeed, velocity.z);
         if (glm::length(desired) > 0.001f) {
             desired = glm::normalize(desired) * maxSpeed;
             glm::vec3 steerUp = desired - velocity;
             float strength = glm::clamp(1.0f - (distanceToFloor / 3.0f), 0.0f, 1.0f);
             steer += limit(steerUp, maxForce * 2.5f) * strength;
         }
    }

    return steer;
}

glm::vec3 FishBoid::calculatePlayerInteraction(Transform& cameraTransform) {
    glm::vec3 steer(0.0f);
    glm::vec3 globalPos = getGlobalTransform().position;

    const auto& players = getNodesInGroup("player");
    glm::vec3 targetPos = cameraTransform.position;

    if (!players.empty()) {
        targetPos = players[0]->getGlobalTransform().position;
    }

    glm::vec3 toPlayer = targetPos - globalPos;
    float distance = glm::length(toPlayer);

    if (distance < 0.001f) return steer;

    glm::vec3 dirToPlayer = glm::normalize(toPlayer);

    if (distance < 12.0f) {
        glm::vec3 desiredFlee = -dirToPlayer * maxSpeed;
        steer += limit(desiredFlee - velocity, maxForce * 2.5f) * 2.0f;
        return steer;
    }

    FishFlock* flockManager = dynamic_cast<FishFlock*>(parent);
    if (!flockManager || !flockManager->targetPath) {
        if (distance > targetOrbitRadius + 5.0f) {
            glm::vec3 desiredAttract = dirToPlayer * maxSpeed;
            steer += limit(desiredAttract - velocity, maxForce) * playerAttractionWeight;
        }

        glm::vec3 orbitDir = glm::normalize(glm::cross(dirToPlayer, glm::vec3(0, 1, 0)));
        if (glm::length(orbitDir) > 0.001f) {
            glm::vec3 desiredOrbit = orbitDir * maxSpeed;
            steer += limit(desiredOrbit - velocity, maxForce) * playerOrbitWeight;
        }
    }

    return steer;
}

glm::vec3 FishBoid::calculateSeparation(const std::vector<Wesen*>& neighbors) {
    glm::vec3 steer(0.0f);
    int count = 0;
    for (Wesen* other : neighbors) {
        if (other == this) continue;
        float d = glm::distance(transform.position, other->transform.position);
        if (d > 0.001f && d < separationRadius) {
            glm::vec3 diff = transform.position - other->transform.position;
            if (glm::length(diff) > 0.001f) {
                diff = glm::normalize(diff) / d;
                steer += diff;
                count++;
            }
        }
    }
    if (count > 0) {
        steer /= (float)count;
        if (glm::length(steer) > 0.001f) {
            steer = glm::normalize(steer) * maxSpeed;
            steer -= velocity;
            steer = limit(steer, maxForce);
        }
    }
    return steer;
}

glm::vec3 FishBoid::calculateAlignment(const std::vector<Wesen*>& neighbors) {
    glm::vec3 sum(0.0f);
    int count = 0;
    for (Wesen* other : neighbors) {
        if (other == this) continue;
        float d = glm::distance(transform.position, other->transform.position);
        if (d > 0.001f && d < neighborRadius) {
            FishBoid* boid = dynamic_cast<FishBoid*>(other);
            if (boid) {
                sum += boid->velocity;
                count++;
            }
        }
    }
    if (count > 0) {
        sum /= (float)count;
        if (glm::length(sum) > 0.001f) {
            sum = glm::normalize(sum) * maxSpeed;
            glm::vec3 steer = sum - velocity;
            return limit(steer, maxForce);
        }
    }
    return glm::vec3(0.0f);
}

glm::vec3 FishBoid::calculateCohesion(const std::vector<Wesen*>& neighbors) {
    glm::vec3 sum(0.0f);
    int count = 0;
    for (Wesen* other : neighbors) {
        if (other == this) continue;
        float d = glm::distance(transform.position, other->transform.position);
        if (d > 0.001f && d < neighborRadius) {
            sum += other->transform.position;
            count++;
        }
    }
    if (count > 0) {
        sum /= (float)count;
        glm::vec3 desired = sum - transform.position;
        if (glm::length(desired) > 0.001f) {
            desired = glm::normalize(desired) * maxSpeed;
            glm::vec3 steer = desired - velocity;
            return limit(steer, maxForce);
        }
    }
    return glm::vec3(0.0f);
}

glm::vec3 FishBoid::limit(const glm::vec3& v, float maxVal) {
    if (glm::length(v) > maxVal) {
        return glm::normalize(v) * maxVal;
    }
    return v;
}