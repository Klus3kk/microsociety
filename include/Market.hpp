#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include "Player.hpp"
#include "Object.hpp"
#include "debug.hpp" // Include your debug console header

class Market : public Object {
private:
    std::unordered_map<std::string, float> prices;      // Current item prices
    std::unordered_map<std::string, int> demand;        // Demand for each item
    std::unordered_map<std::string, int> supply;        // Supply for each item
    std::unordered_map<std::string, std::vector<float>> priceHistory;

    const float priceAdjustmentFactor = 0.2f; // Scaling factor for price updates
    const float minimumPrice = 1.0f;          // Minimum price for any item

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

    // Initialize prices and supply-demand values for items
    void setPrice(const std::string& item, float price) {
        prices[item] = price;
        demand[item] = 50;
        supply[item] = 100; // Default supply
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

            demand[item] += quantity;
            supply[item] = std::max(0, supply[item] - quantity); // Decrease supply

            updatePrice(item);

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

            supply[item] += quantity;
            updatePrice(item);

            getDebugConsole().log("Market", "Sold " + std::to_string(quantity) + " " + item + 
                                  "(s) for $" + std::to_string(revenue));
            return true;
        }

        getDebugConsole().log("Market", "Not enough " + item + " to sell.");
        return false;
    }
    const std::unordered_map<std::string, std::vector<float>>& getPriceTrendMap() const {
        return priceHistory; // Assuming `priceHistory` tracks the trends
    }

    float adjustPrice(float currentPrice, int demand, int supply, float elasticity, float momentum) {
        if (supply == 0) supply = 1; // Prevent division by zero
        float demandSupplyRatio = static_cast<float>(demand) / supply;

        // Amplify the base adjustment to prioritize demand's effect
        float baseAdjustment = momentum * (demandSupplyRatio - 1.0f);

        // Adjust the price using a stronger demand influence
        float adjustedPrice = currentPrice * (1.0f + elasticity * baseAdjustment);

        // Cap adjustments for stability
        adjustedPrice = std::clamp(adjustedPrice, currentPrice * 0.7f, currentPrice * 1.7f);

        // Ensure price never falls below the minimum
        return std::max(minimumPrice, adjustedPrice);
    }

    // Update price dynamically based on supply and demand
    void updatePrice(const std::string& item) {
        int itemDemand = demand[item];
        int itemSupply = supply[item];

        // Update price using the greatness formula
        float elasticity = 0.2f;  // Adjust for faster/slower changes
        float momentum = 0.5f;    // Add inertia to pricing
        prices[item] = adjustPrice(prices[item], itemDemand, itemSupply, elasticity, momentum);

        // Track history for visualization
        trackPriceHistory(item);

        // Debug log
        getDebugConsole().log("Market", "Updated price for " + item +
                            " | New Price: " + std::to_string(prices[item]) +
                            " | Demand: " + std::to_string(itemDemand) +
                            " | Supply: " + std::to_string(itemSupply));
        trackPriceHistory(item);
    }



    void trackPriceHistory(const std::string& item) {
        priceHistory[item].push_back(prices[item]);
        if (priceHistory[item].size() > 10) {
            priceHistory[item].erase(priceHistory[item].begin());
        }

        // Debug log for trend tracking
        getDebugConsole().log("Market", "TrackPriceHistory: " + item +
                            " | Current Price: " + std::to_string(prices[item]) +
                            " | History Size: " + std::to_string(priceHistory[item].size()));
    }




    // Display current market prices for debugging or shop interface
    void displayPrices() const {
        getDebugConsole().log("Market", "Current Market Prices:");
        for (const auto& [item, price] : prices) {
            getDebugConsole().log("Market", "- " + item + ": $" + std::to_string(price));
        }
    }

    std::vector<float> getPriceTrend(const std::string& item) const {
        auto it = priceHistory.find(item);
        if (it != priceHistory.end()) {
            return it->second;
        }
        return {};
    }


    // Implement pure virtual methods from Object
    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    ObjectType getType() const override {
        return ObjectType::Market; // Returns the type as Market
    }
};

#endif // MARKET_HPP
