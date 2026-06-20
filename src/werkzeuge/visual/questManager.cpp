//
// Created by Alexey Pravilov on 20/06/2026.
//

#include "questManager.h"

void QuestManager::acceptQuest(const Quest& quest) {
    activeQuests.push_back(quest);
    if (onProgressChanged) {
        onProgressChanged();
    }
}

void QuestManager::progressObjective(const std::string& tag, int amount) {
    bool changed = false;

    for (auto& quest : activeQuests) {
        if (quest.isCompleted) continue;

        bool allObjectivesDone = true;
        for (auto& obj : quest.objectives) {
            if (obj.tag == tag && !obj.isCompleted) {
                obj.progress(amount);
                changed = true;
            }
            if (!obj.isCompleted) {
                allObjectivesDone = false;
            }
        }

        if (allObjectivesDone) {
            quest.isCompleted = true;
            if (quest.onComplete) {
                quest.onComplete();
            }
        }
    }

    if (changed && onProgressChanged) {
        onProgressChanged();
    }
}