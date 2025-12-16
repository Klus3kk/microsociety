#ifndef MONEYMANAGER_HPP
#define MONEYMANAGER_HPP

#include "NPCEntity.hpp"
#include <vector>
#include <numeric>

class MoneyManager {
public:
    // calculate the total money held by all NPCs
    static int calculateTotalMoney(const std::vector<NPCEntity>& npcs) {
        return std::accumulate(npcs.begin(), npcs.end(), 0,
                               [](int total, const NPCEntity& npc) {
                                   return total + npc.getMoney();
                               });
    }

    // track money spent and earned
    static void recordMoneySpent(int amount) { totalMoneySpent += amount; }
    static void recordMoneyEarned(int amount) { totalMoneyEarned += amount; }

    // get total money spent and earned
    static int getTotalMoneySpent() { return totalMoneySpent; }
    static int getTotalMoneyEarned() { return totalMoneyEarned; }

private:
    static inline int totalMoneySpent = 0;  // Define inside the class with `inline`
    static inline int totalMoneyEarned = 0; // Define inside the class with `inline`
};

#endif
