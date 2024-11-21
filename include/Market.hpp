#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <string>
#include "Player.hpp"

class Market {
private:
    std::unordered_map<std::string, float> prices;      // Item prices
    std::unordered_map<std::string, int> supplyDemand;  // Supply/Demand tracking

    // Adjust price dynamically based on supply and demand
    void adjustPrice(const std::string& item) {
        int demand = supplyDemand[item];
        prices[item] += demand * 0.5f; // Example: Increase price by 0.5 per demand unit
        prices[item] = std::max(1.0f, prices[item]); // Ensure price doesn't drop below 1.0
    }

public:
    Market() {}

    void setPrice(const std::string& item, float price) {
        prices[item] = price;
        supplyDemand[item] = 0; // Initialize demand/supply
    }

    float getPrice(const std::string& item) const {
        auto it = prices.find(item);
        return it != prices.end() ? it->second : 0.0f;
    }

    bool buyItem(PlayerEntity& player, const std::string& item, int quantity) {
        float price = getPrice(item) * quantity;

        if (player.getMoney() >= price) {
            player.setMoney(player.getMoney() - price);
            player.addToInventory(item, quantity);

            // Update supply/demand
            supplyDemand[item] += quantity;
            adjustPrice(item);

            return true;
        }
        return false;
    }

    bool sellItem(PlayerEntity& player, const std::string& item, int quantity) {
        if (player.getInventoryItemCount(item) >= quantity) {
            float price = getPrice(item) * quantity;

            player.removeFromInventory(item, quantity);
            player.setMoney(player.getMoney() + price);

            // Update supply/demand
            supplyDemand[item] -= quantity;
            adjustPrice(item);

            return true;
        }
        return false;
    }
};

#endif // MARKET_HPP
