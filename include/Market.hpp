#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Object.hpp"
#include "debug.hpp"
#include <cmath>      
#include <numeric>    

class Market : public Object {
private:
    std::unordered_map<std::string, float> prices;
    std::unordered_map<std::string, int> demand;
    std::unordered_map<std::string, int> supply;
    std::unordered_map<std::string, std::vector<float>> priceHistory;
    std::unordered_map<std::string, int> totalBuyTransactions;
    std::unordered_map<std::string, int> totalSellTransactions;
    std::unordered_map<std::string, float> totalRevenue;
    std::unordered_map<std::string, float> totalExpenditure;

    const float priceAdjustmentFactor = 0.2f;
    const float minimumPrice = 1.0f;
    float sellMargin = 0.9f;
    float buyMargin = 1.1f;

public:
    Market();
    Market(const sf::Texture& tex);

    void setPrice(const std::string& item, float price);
    float getPrice(const std::string& item) const;
    float calculateBuyPrice(const std::string& item) const;
    float calculateSellPrice(const std::string& item) const;

    bool buyItem(PlayerEntity& player, const std::string& item, int quantity);
    bool sellItem(PlayerEntity& player, const std::string& item, int quantity);

    float adjustPriceOnBuy(float currentPrice, int demand, int supply, float buyFactor);
    float adjustPriceOnSell(float currentPrice, int demand, int supply, float sellFactor);

    void trackPriceHistory(const std::string& item);
    float calculateVolatility(const std::string& item) const;
    void displayPrices() const;
    void stabilizePrices(float deltaTime);
    std::unordered_map<std::string, float> getResourceStats() const;

    std::vector<float> getPriceTrend(const std::string& item) const;
    const std::unordered_map<std::string, std::vector<float>>& getPriceTrendMap() const; // Added for UI
    const std::unordered_map<std::string, float>& getPrices() const;                     // Added for UI

    int getTotalTrades() const;                          // Added for UI
    int getBuyTransactions(const std::string& item) const;
    int getSellTransactions(const std::string& item) const;
    float getRevenue(const std::string& item) const;
    float getExpenditure(const std::string& item) const;

    void renderPriceGraph(sf::RenderWindow& window, const std::string& item, sf::Vector2f position, sf::Vector2f size);

    void draw(sf::RenderWindow& window) override;
    ObjectType getType() const override;
};

#endif // MARKET_HPP
