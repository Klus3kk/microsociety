#include "MoneyManager.hpp"

int MoneyManager::calculateTotalMoney(const std::vector<PlayerEntity>& npcs) {
    float totalMoney = 0.0f;

    for (const auto& npc : npcs) {
        totalMoney += npc.getMoney();
    }

    return static_cast<int>(totalMoney);
}
