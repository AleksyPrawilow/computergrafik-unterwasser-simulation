//
// Created by Alexey Pravilov on 10/06/2026.
//

#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GROUPMANAGER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GROUPMANAGER_H
#include "wesen.h"
#include <string>
#include <vector>
#include <unordered_map>

class Wesen;

class GroupManager {
public:
    static GroupManager& getInstance() {
        static GroupManager instance;
        return instance;
    }

    void addToGroup(const std::string& groupName, Wesen * wesen);
    void removeFromGroup(const std::string& groupName, Wesen * wesen);
    const std::vector<Wesen * >& getEntitiesInGroup(const std::string& groupName);
    void cleanup();

private:
    GroupManager() = default;
    std::unordered_map<std::string, std::vector<Wesen*>> groups {};
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_GROUPMANAGER_H
