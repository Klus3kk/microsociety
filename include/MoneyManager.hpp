#ifndef MONEYMANAGER_HPP
#define MONEYMANAGER_HPP

#include "Player.hpp"
#include <vector>

class MoneyManager {
public:
    // Calculate the total money among all NPCs
    static int calculateTotalMoney(const std::vector<PlayerEntity>& npcs);
};

#endif
