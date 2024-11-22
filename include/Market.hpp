#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <string>
#include "Player.hpp"
#include "Object.hpp"
#include "debug.hpp" // Include your debug console header

class Market : public Object {
private:
    std::unordered_map<std::string, float> prices;      // Current item prices
    std::unordered_map<std::string, int> supplyDemand;  // Supply/Demand tracking

    // Adjust prices dynamically based on supply/demand
    void adjustPrice(const std::string& item) {
        int demand = supplyDemand[item];
        prices[item] += 0.1f * demand;
        prices[item] = std::max(1.0f, prices[item]); // Enforce minimum price of 1.0
    }

public:
    Market() {
        // Blank texture
        sf::Texture defaultTexture;
        texture = defaultTexture;
        sprite.setTexture(texture);
    }

    Market(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    // Implement pure virtual methods from Object
    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    ObjectType getType() const override {
        return ObjectType::Market; // Returns the type as Market
    }

    // Set initial prices and supply/demand
    void setPrice(const std::string& item, float price) {
        prices[item] = price;
        supplyDemand[item] = 0;
    }

    float getPrice(const std::string& item) const {
        auto it = prices.find(item);
        return it != prices.end() ? it->second : 0.0f;
    }

    // Buy item from the market
    bool buyItem(PlayerEntity& player, const std::string& item, int quantity) {
        if (prices.find(item) == prices.end()) {
            getDebugConsole().log("Market", "Item not available in market: " + item);
            return false;
        }

        float cost = getPrice(item) * quantity;

        if (player.getMoney() >= cost) {
            player.setMoney(player.getMoney() - cost);
            player.addToInventory(item, quantity);

            supplyDemand[item] += quantity; // Increase demand
            adjustPrice(item);              // Adjust price

            getDebugConsole().log("Market", "Bought " + std::to_string(quantity) + " " + item + 
                                  "(s) for $" + std::to_string(cost));
            return true;
        }

        getDebugConsole().log("Market", "Not enough money to buy " + item);
        return false;
    }

    // Sell item to the market
    bool sellItem(PlayerEntity& player, const std::string& item, int quantity) {
        if (prices.find(item) == prices.end()) {
            getDebugConsole().log("Market", "Item not accepted by market: " + item);
            return false;
        }

        if (player.getInventoryItemCount(item) >= quantity) {
            float revenue = getPrice(item) * quantity;

            player.removeFromInventory(item, quantity);
            player.setMoney(player.getMoney() + revenue);

            supplyDemand[item] -= quantity; // Reduce demand
            adjustPrice(item);              // Adjust price

            getDebugConsole().log("Market", "Sold " + std::to_string(quantity) + " " + item + 
                                  "(s) for $" + std::to_string(revenue));
            return true;
        }

        getDebugConsole().log("Market", "Not enough " + item + " to sell.");
        return false;
    }

    // Display current market prices (for debugging or shop interface)
    void displayPrices() const {
        getDebugConsole().log("Market", "Current Market Prices:");
        for (const auto& [item, price] : prices) {
            getDebugConsole().log("Market", "- " + item + ": $" + std::to_string(price));
        }
    }
};

#endif
