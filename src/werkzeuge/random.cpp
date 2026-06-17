//
// Created by Alexey Pravilov on 17/06/2026.
//

#include "random.h"

#include <random>

namespace Random {
    float randf() {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(rng);
    }

    float range(float min, float max) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }
}
