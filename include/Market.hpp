#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"  
#include "Object.hpp"
#include "debug.hpp"
#include <cmath>
#include <numeric>

class NPCEntity;
class PlayerEntity;


// Market class: Represents a dynamic in-game trading system
class Market : public Object {
private:
    std::unordered_map<std::string, float> prices;          // Stores the current market price for each item
    std::unordered_map<std::string, int> demand;           // Tracks the demand level for each item
    std::unordered_map<std::string, int> supply;           // Tracks the supply level for each item
    std::unordered_map<std::string, std::vector<float>> priceHistory; // Stores historical price trends
    std::unordered_map<std::string, int> totalBuyTransactions; // Tracks total buy transactions for each item
    std::unordered_map<std::string, int> totalSellTransactions; // Tracks total sell transactions for each item
    std::unordered_map<std::string, float> totalRevenue;   // Tracks total revenue generated from each item
    std::unordered_map<std::string, float> totalExpenditure; // Tracks total expenditure on each item

    const float priceAdjustmentFactor = 0.2f;              // Controls how price fluctuates with supply/demand
    const float minimumPrice = 1.0f;                       // The lowest possible price for any item
    const float maximumPrice = 100.0f;                     // The highest possible price for any item
    float sellMargin = 0.9f;                               // Selling price multiplier (lower than buy price)
    float buyMargin = 1.1f;                                // Buying price multiplier (higher than sell price)

public:
    // Constructors
    Market();
    Market(const sf::Texture& tex);

    // Set and Get Prices
    void setPrice(const std::string& item, float price); // Sets a specific item's price
    float getPrice(const std::string& item) const;       // Retrieves the current price of an item
    float calculateBuyPrice(const std::string& item) const;  // Determines price when buying
    float calculateSellPrice(const std::string& item) const; // Determines price when selling

    // FIXED: Core Transactions - now accept Entity base class (works with both NPCs and Players)
    bool buyItem(Entity& player, const std::string& item, int quantity); // Handles entity purchasing an item
    bool sellItem(Entity& player, const std::string& item, int quantity); // Handles entity selling an item

    // Adjust Prices Dynamically
    float adjustPriceOnBuy(float currentPrice, int demand, int supply, float buyFactor); // Modify price on purchase
    float adjustPriceOnSell(float currentPrice, int demand, int supply, float sellFactor); // Modify price on sale

    // Price Tracking & Market Trends
    void trackPriceHistory(const std::string& item);  // Records price changes over time
    float calculateVolatility(const std::string& item) const; // Measures price fluctuations
    std::vector<float> getPriceTrend(const std::string& item) const; // Retrieves price history
    const std::unordered_map<std::string, std::vector<float>>& getPriceTrendMap() const; // Returns full market trend data
    const std::unordered_map<std::string, float>& getPrices() const; // Returns all current prices

    // Market Statistics
    int getTotalTrades() const;                          // Gets total number of trades
    int getBuyTransactions(const std::string& item) const; // Retrieves total purchases of an item
    int getSellTransactions(const std::string& item) const; // Retrieves total sales of an item
    int getTotalItemsSold() const;  // Calculates total number of items sold
    int getTotalItemsBought() const; // Calculates total number of items bought
    float getRevenue(const std::string& item) const; // Returns total revenue from an item
    float getExpenditure(const std::string& item) const; // Returns total money spent on an item

    // Market Intelligence (AI Recommendations)
    std::string suggestBestResourceToBuy() const;  // Suggests best resource to buy based on profitability
    std::string suggestBestResourceToSell() const; // Suggests best resource to sell based on market trends

    // Dynamic Market Adjustments
    void stabilizePrices(float deltaTime);        // Slowly stabilizes market prices over time
    void simulateMarketDynamics(float deltaTime); // Simulates price variations due to economic forces
    void resetTransactions(); // Resets all transaction history
    void randomizePrices();   // Introduces random fluctuations in prices

    // UI and Rendering
    void renderPriceGraph(sf::RenderWindow& window, const std::string& item, sf::Vector2f position, sf::Vector2f size) const; // Renders price trends
    void displayPrices() const; // Prints prices to console (for debugging)
    void draw(sf::RenderWindow& window) override; // Renders the market visually
    ObjectType getType() const override; // Returns the type of object (Market)
    std::unordered_map<std::string, float> getResourceStats() const; // Provides current resource prices for UI

};

#endif