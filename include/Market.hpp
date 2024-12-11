#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "NPCEntity.hpp"
#include "Object.hpp"
#include "debug.hpp"
#include <cmath>
#include <numeric>

class Market : public Object {
private:
    std::unordered_map<std::string, float> prices;          // Current prices for items
    std::unordered_map<std::string, int> demand;           // Demand levels for items
    std::unordered_map<std::string, int> supply;           // Supply levels for items
    std::unordered_map<std::string, std::vector<float>> priceHistory; // Price history for trend analysis
    std::unordered_map<std::string, int> totalBuyTransactions;
    std::unordered_map<std::string, int> totalSellTransactions;
    std::unordered_map<std::string, float> totalRevenue;
    std::unordered_map<std::string, float> totalExpenditure;

    const float priceAdjustmentFactor = 0.2f;              // Controls how price adjusts with supply/demand
    const float minimumPrice = 1.0f;                       // Minimum allowed price
    const float maximumPrice = 100.0f;                     // Maximum allowed price
    float sellMargin = 0.9f;                               // Margin multiplier for sell prices
    float buyMargin = 1.1f;                                // Margin multiplier for buy prices

public:
    Market();
    Market(const sf::Texture& tex);

    // Setters and Getters
    void setPrice(const std::string& item, float price);
    float getPrice(const std::string& item) const;
    float calculateBuyPrice(const std::string& item) const;
    float calculateSellPrice(const std::string& item) const;

    // Core Transactions
    bool buyItem(PlayerEntity& player, const std::string& item, int quantity);
    bool sellItem(PlayerEntity& player, const std::string& item, int quantity);

    // Price Adjustments
    float adjustPriceOnBuy(float currentPrice, int demand, int supply, float buyFactor);
    float adjustPriceOnSell(float currentPrice, int demand, int supply, float sellFactor);

    // Price Trends and Metrics
    void trackPriceHistory(const std::string& item);
    float calculateVolatility(const std::string& item) const;
    std::vector<float> getPriceTrend(const std::string& item) const;
    const std::unordered_map<std::string, std::vector<float>>& getPriceTrendMap() const; // For UI
    const std::unordered_map<std::string, float>& getPrices() const;                     // For UI

    // Statistics
    int getTotalTrades() const;                          // Total number of trades
    int getBuyTransactions(const std::string& item) const;
    int getSellTransactions(const std::string& item) const;
    float getRevenue(const std::string& item) const;
    float getExpenditure(const std::string& item) const;

    // Market Intelligence for AI
    std::string suggestBestResourceToBuy() const;         // Suggest resource with the highest ROI
    std::string suggestBestResourceToSell() const;        // Suggest resource with the best profit margin
    bool isResourceAvailable(const std::string& item, int quantity) const; // Check if a resource is available

    // Dynamic Market Adjustments
    void stabilizePrices(float deltaTime);               // Adjust prices to stabilize market
    void simulateMarketDynamics(float deltaTime);        // Simulate changes in demand and supply over time

    // UI and Rendering
    void renderPriceGraph(sf::RenderWindow& window, const std::string& item, sf::Vector2f position, sf::Vector2f size) const;
    void displayPrices() const;
    void draw(sf::RenderWindow& window) override;
    ObjectType getType() const override;
};

#endif // MARKET_HPP
