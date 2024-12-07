#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>
#include "Player.hpp"
#include "Object.hpp"
#include "debug.hpp" 

class Market : public Object {
private:
    std::unordered_map<std::string, float> prices;      // Current item prices
    std::unordered_map<std::string, int> demand;        // Demand for each item
    std::unordered_map<std::string, int> supply;        // Supply for each item
    std::unordered_map<std::string, std::vector<float>> priceHistory;
    std::unordered_map<std::string, int> totalBuyTransactions;
    std::unordered_map<std::string, int> totalSellTransactions;
    std::unordered_map<std::string, float> totalRevenue;   // Total money earned from selling
    std::unordered_map<std::string, float> totalExpenditure; // Total money spent on buying


    const float priceAdjustmentFactor = 0.2f; // Scaling factor for price updates
    const float minimumPrice = 1.0f;          // Minimum price for any item
    float sellMargin = 0.9f; // Selling price is 90% of buying price
    float buyMargin = 1.1f;  // Buying price is 10% higher than base price

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
        if (prices.find(item) == prices.end()) {
            prices[item] = price;
            demand[item] = 50; // Default demand
            supply[item] = 100; // Default supply
        }
    }

    float getPrice(const std::string& item) const {
        auto it = prices.find(item);
        return it != prices.end() ? it->second : 0.0f;
    }

    float calculateBuyPrice(const std::string& item) const {
        return std::round(getPrice(item) * buyMargin * 10) / 10.0f; // Ensure consistent rounding
    }

    float calculateSellPrice(const std::string& item) const {
        return std::round(getPrice(item) * sellMargin * 10) / 10.0f; // Ensure consistent rounding
    }

    bool buyItem(PlayerEntity& player, const std::string& item, int quantity) {
        if (prices.find(item) == prices.end()) setPrice(item, 10.0f);

        float cost = calculateBuyPrice(item) * quantity;
        if (player.getMoney() >= cost) {
            player.setMoney(player.getMoney() - cost);
            player.addToInventory(item, quantity);

            demand[item] += quantity;
            supply[item] = std::max(0, supply[item] - quantity);
            prices[item] = adjustPriceOnBuy(prices[item], demand[item], supply[item], 0.3f);

            trackPriceHistory(item);

            // Advanced Stats
            totalBuyTransactions[item] += quantity;
            totalExpenditure[item] += cost;

            return true;
        }
        return false;
    }

    bool sellItem(PlayerEntity& player, const std::string& item, int quantity) {
        if (prices.find(item) == prices.end()) setPrice(item, 10.0f);

        if (player.getInventoryItemCount(item) >= quantity) {
            float revenue = calculateSellPrice(item) * quantity;
            player.removeFromInventory(item, quantity);
            player.setMoney(player.getMoney() + revenue);

            supply[item] += quantity;
            demand[item] = std::max(0, demand[item] - quantity);
            prices[item] = adjustPriceOnSell(prices[item], demand[item], supply[item], 0.2f);

            trackPriceHistory(item);

            // Advanced Stats
            totalSellTransactions[item] += quantity;
            totalRevenue[item] += revenue;

            return true;
        }
        return false;
    }

    float calculateVolatility(const std::string& item) const {
        // Check if item exists in priceHistory
        auto it = priceHistory.find(item);
        if (it == priceHistory.end() || it->second.size() < 2) {
            getDebugConsole().log("Market", "Volatility calculation skipped: Insufficient history for " + item);
            return 0.0f;
        }

        const auto& history = it->second;
        float mean = std::accumulate(history.begin(), history.end(), 0.0f) / history.size();
        float variance = 0.0f;

        for (float price : history) {
            variance += std::pow(price - mean, 2);
        }
        variance /= history.size();

        return std::sqrt(variance); // Standard deviation
    }


    const std::unordered_map<std::string, float>& getPrices() const {
        return prices;
    }

    const std::unordered_map<std::string, std::vector<float>>& getPriceTrendMap() const {
        return priceHistory; 
    }

    float adjustPriceOnBuy(float currentPrice, int demand, int supply, float buyFactor) {
        if (supply == 0) supply = 1;  // Avoid division by zero

        float adjustmentFactor = buyFactor * (static_cast<float>(demand) / supply);
        float adjustedPrice = currentPrice * (1.0f + adjustmentFactor);

        getDebugConsole().log("Market", "Buy Adjustment: currentPrice=" + std::to_string(currentPrice) +
                            ", demand=" + std::to_string(demand) + ", supply=" + std::to_string(supply) +
                            ", adjustedPrice=" + std::to_string(adjustedPrice));

        return std::clamp(adjustedPrice, minimumPrice, currentPrice * 2.0f);
    }

    float adjustPriceOnSell(float currentPrice, int demand, int supply, float sellFactor) {
        if (demand == 0) demand = 1;  // Avoid division by zero

        float adjustmentFactor = -sellFactor * (static_cast<float>(supply) / demand);
        float adjustedPrice = currentPrice * (1.0f + adjustmentFactor);

        getDebugConsole().log("Market", "Sell Adjustment: currentPrice=" + std::to_string(currentPrice) +
                            ", demand=" + std::to_string(demand) + ", supply=" + std::to_string(supply) +
                            ", adjustedPrice=" + std::to_string(adjustedPrice));

        return std::clamp(adjustedPrice, minimumPrice, currentPrice * 2.0f);
    }




    void updatePrice(const std::string& item) {
        if (prices.find(item) == prices.end()) return;

        // Prices are already adjusted in buyItem/sellItem; track price history only
        trackPriceHistory(item);

        getDebugConsole().log("Market", "Price tracked for " + item + ": $" + std::to_string(prices[item]) +
                                                " | Demand: " + std::to_string(demand[item]) +
                                                " | Supply: " + std::to_string(supply[item]));
    }



    void stabilizePrices(float deltaTime) {
        for (auto& [item, price] : prices) {
            // Gradually stabilize prices toward a midpoint
            float demandSupplyRatio = static_cast<float>(demand[item]) / (supply[item] + 1);
            float targetPrice = 10.0f * (1.0f + (demandSupplyRatio - 1.0f) * 0.05f); // Midpoint price logic
            prices[item] += (targetPrice - price) * deltaTime * 0.1f; // Smooth adjustment

            // Track stabilization in price history
            trackPriceHistory(item);
        }
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
            getDebugConsole().log("Market", "- " + item + ": Buy Price: $" + std::to_string(calculateBuyPrice(item)) +
                                                ", Sell Price: $" + std::to_string(calculateSellPrice(item)));
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

    void renderPriceGraph(sf::RenderWindow& window, const std::string& item, sf::Vector2f position, sf::Vector2f size) {
        auto& history = priceHistory[item];
        if (history.empty()) return;

        sf::VertexArray graph(sf::LineStrip, history.size());

        float maxPrice = *std::max_element(history.begin(), history.end());
        float minPrice = *std::min_element(history.begin(), history.end());

        for (size_t i = 0; i < history.size(); ++i) {
            float normalizedPrice = (history[i] - minPrice) / (maxPrice - minPrice);
            float x = position.x + (i / static_cast<float>(history.size())) * size.x;
            float y = position.y + size.y - (normalizedPrice * size.y);
            graph[i].position = sf::Vector2f(x, y);
            graph[i].color = sf::Color::Green; // Customize graph color
        }

        window.draw(graph);
    }


    ObjectType getType() const override {
        return ObjectType::Market; // Returns the type as Market
    }

    int getTotalTrades() const {
        int total = 0;
        for (const auto& [item, qty] : supply) {
            total += qty;
        }
        return total;
    }

    std::unordered_map<std::string, float> getResourceStats() const {
        std::unordered_map<std::string, float> stats;
        for (const auto& [item, price] : prices) {
            stats[item] = price;
        }
        return stats;
    }

    int getBuyTransactions(const std::string& item) const {
        auto it = totalBuyTransactions.find(item);
        return it != totalBuyTransactions.end() ? it->second : 0;
    }

    int getSellTransactions(const std::string& item) const {
        auto it = totalSellTransactions.find(item);
        return it != totalSellTransactions.end() ? it->second : 0;
    }

    float getRevenue(const std::string& item) const {
        auto it = totalRevenue.find(item);
        return it != totalRevenue.end() ? it->second : 0.0f;
    }

    float getExpenditure(const std::string& item) const {
        auto it = totalExpenditure.find(item);
        return it != totalExpenditure.end() ? it->second : 0.0f;
    }

    float getSupplyDemandRatio(const std::string& item) const {
        auto supplyIt = supply.find(item);
        auto demandIt = demand.find(item);
        if (supplyIt != supply.end() && demandIt != demand.end() && supplyIt->second > 0) {
            return static_cast<float>(demandIt->second) / supplyIt->second;
        }
        return 0.0f;
    }


};

#endif // MARKET_HPP
