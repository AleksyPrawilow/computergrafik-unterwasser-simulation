//
// Created by Alexey Pravilov on 14/06/2026.
//

#include "worldEnvironment.h"

void WorldEnvironment::init() {
    activeEnv = this;
}

WorldEnvironment::~WorldEnvironment() {
    if (activeEnv == this) activeEnv = nullptr;
}
