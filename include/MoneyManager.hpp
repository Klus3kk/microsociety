#ifndef MONEYMANAGER_HPP
#define MONEYMANAGER_HPP

#include "Player.hpp"
#include <vector>
#include <numeric>

class MoneyManager {
public:
    // Calculate the total money held by all NPCs
    static int calculateTotalMoney(const std::vector<PlayerEntity>& npcs) {
        return std::accumulate(npcs.begin(), npcs.end(), 0,
                               [](int total, const PlayerEntity& npc) {
                                   return total + npc.getMoney();
                               });
    }
};

#endif
