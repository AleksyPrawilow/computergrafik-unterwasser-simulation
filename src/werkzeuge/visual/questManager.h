//
// Created by Alexey Pravilov on 20/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTMANAGER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTMANAGER_H
#include <functional>
#include <string>
#include <utility>
#include <vector>


struct QuestObjective {
    std::string tag;
    std::string description;
    int requiredCount = 1;
    int currentCount = 0;
    bool isCompleted = false;

    void progress(int amount) {
        if (isCompleted) return;
        currentCount = std::min(currentCount + amount, requiredCount);
        if (currentCount >= requiredCount) {
            isCompleted = true;
        }
    }
};

struct Quest {
    std::string title;
    std::vector<QuestObjective> objectives;
    bool isCompleted = false;
    std::function<void()> onComplete;
};

class QuestManager {
public:
    static QuestManager& getInstance() {
        static QuestManager instance;
        return instance;
    }

    void acceptQuest(const Quest& quest);

    void progressObjective(const std::string& tag, int amount = 1);

    [[nodiscard]] const std::vector<Quest>& getActiveQuests() const { return activeQuests; }

    void setOnProgressChangedCallback(std::function<void()> callback) {
        onProgressChanged = std::move(callback);
    }

private:
    QuestManager() = default;

    std::vector<Quest> activeQuests;
    std::function<void()> onProgressChanged = nullptr;
};

#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_QUESTMANAGER_H
