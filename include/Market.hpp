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
    void Market::adjustPrice(const std::string& item) {
        int demand = supplyDemand[item];
        prices[item] += 0.1f * demand;  // Increase price slightly based on demand
        prices[item] = std::max(1.0f, prices[item]); // Ensure minimum price is 1.0
        std::cout << "Adjusted price for " << item << ": " << prices[item] << std::endl;
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

            supplyDemand[item] -= quantity;
            adjustPrice(item);

            return true;
        }
        return false;
    }
};

#endif 
