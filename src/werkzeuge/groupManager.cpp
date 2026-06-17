//
// Created by Alexey Pravilov on 10/06/2026.
//

#include "groupManager.h"

void GroupManager::addToGroup(const std::string& groupName, Wesen* wesen) {
    if (auto& list = groups[groupName]; std::find(list.begin(), list.end(), wesen) == list.end()) {
        list.push_back(wesen);
    }
}

void GroupManager::removeFromGroup(const std::string& groupName, Wesen* wesen) {
    if (const auto it = groups.find(groupName); it != groups.end()) {
        auto& list = it->second;
        list.erase(std::remove(list.begin(), list.end(), wesen), list.end());
    }
}

const std::vector<Wesen*>& GroupManager::getEntitiesInGroup(const std::string& groupName) {
    return groups[groupName];
}

void GroupManager::cleanup() {
    groups.clear();
}
