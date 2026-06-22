#include "fishBolid.h"
#include "werkzeuge/groupManager.h"

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
    glm::vec3 avo = calculateAvoidance(cameraTransform);

    acceleration += sep * separationWeight;
    acceleration += ali * alignmentWeight;
    acceleration += coh * cohesionWeight;
    acceleration += avo * avoidanceWeight;

    velocity += acceleration * deltaTime;
    velocity = limit(velocity, maxSpeed);
    transform.position += velocity * deltaTime;

    if (glm::length(velocity) > 0.01f) {
        transform.lookAt(transform.position + velocity);
    }
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
        if (d > 0 && d < neighborRadius) {
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
        if (d > 0 && d < neighborRadius) {
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

glm::vec3 FishBoid::calculateAvoidance(Transform& cameraTransform) {
    glm::vec3 steer(0.0f);

    glm::vec3 globalPos = getGlobalTransform().position;
    float d = glm::distance(globalPos, cameraTransform.position);
    float avoidanceRadius = 8.0f;

    if (d > 0.001f && d < avoidanceRadius) {
        glm::vec3 diff = globalPos - cameraTransform.position;
        if (glm::length(diff) > 0.001f) {
            diff = glm::normalize(diff) * maxSpeed;
            steer = diff - velocity;
            steer = limit(steer, maxForce * 2.0f);
        }
    }
    return steer;
}

glm::vec3 FishBoid::limit(const glm::vec3& v, float maxVal) {
    if (glm::length(v) > maxVal) {
        return glm::normalize(v) * maxVal;
    }
    return v;
}