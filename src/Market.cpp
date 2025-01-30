#include "Market.hpp"
#include "MoneyManager.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <ctime>
#include <cstdlib>
#include "House.hpp"
// Default constructor
Market::Market() {
    sf::Texture defaultTexture;
    texture = defaultTexture;
    sprite.setTexture(texture);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    setPrice("wood", 1 + std::rand() % 50);
    setPrice("stone", 1 + std::rand() % 50);
    setPrice("bush", 1 + std::rand() % 50);

    // Debugging: Ensure prices are set
    getDebugConsole().log("DEBUG", "Market initialized with prices:");
    for (const auto& [item, price] : prices) {
        getDebugConsole().log("DEBUG", "- " + item + ": $" + std::to_string(price));
    }
}


// Constructor with texture
Market::Market(const sf::Texture& tex) {
    texture = tex;
    sprite.setTexture(texture);
}

// Set price for an item
void Market::setPrice(const std::string& item, float price) {
    if (prices.find(item) == prices.end()) {
        prices[item] = price;
        demand[item] = 50;
        supply[item] = 100;
        totalBuyTransactions[item] = 0;
        totalSellTransactions[item] = 0;
        totalRevenue[item] = 0.0f;
        totalExpenditure[item] = 0.0f;
    }
}

// Get the price of an item
float Market::getPrice(const std::string& item) const {
    auto it = prices.find(item);
    return it != prices.end() ? it->second : 0.0f;
}


// Calculate buy price
float Market::calculateBuyPrice(const std::string& item) const {
    return std::round(getPrice(item) * buyMargin * 10) / 10.0f;
}

// Calculate sell price
float Market::calculateSellPrice(const std::string& item) const {
    return std::round(getPrice(item) * sellMargin * 10) / 10.0f;
}

bool Market::buyItem(NPCEntity& npc, const std::string& item, int quantity) {
    if (quantity <= 0 || item.empty()) return false;
    if (prices.find(item) == prices.end()) setPrice(item, 1 + std::rand() % 50);

    float itemPrice = calculateBuyPrice(item);
    float totalCost = itemPrice * quantity;

    if (npc.getMoney() < totalCost) {  
        getDebugConsole().log("ERROR", npc.getName() + " cannot afford " + std::to_string(quantity) + " " + item);
        return false;
    }

    if (!npc.addToInventory(item, quantity)) {
        getDebugConsole().log("ERROR", npc.getName() + " inventory FULL. Cannot buy " + item);
        return false;
    }

    npc.setMoney(npc.getMoney() - totalCost);
    MoneyManager::recordMoneySpent(totalCost);  // ✅ Track money spent

    demand[item] += quantity;
    supply[item] = std::max(0, supply[item] - quantity);
    prices[item] = adjustPriceOnBuy(prices[item], demand[item], supply[item], priceAdjustmentFactor);

    totalBuyTransactions[item] += quantity;
    totalExpenditure[item] += totalCost;  

    npc.addReward(5.0f * quantity);

    getDebugConsole().log("MARKET", "[BUY] " + npc.getName() + " bought " + std::to_string(quantity) + " " + item);
    return true;
}

void Market::resetTransactions() {
    for (auto& [item, count] : totalBuyTransactions) {
        count = 0;
    }
    for (auto& [item, count] : totalSellTransactions) {
        count = 0;
    }
    for (auto& [item, revenue] : totalRevenue) {
        revenue = 0.0f;
    }
    for (auto& [item, expenditure] : totalExpenditure) {
        expenditure = 0.0f;
    }
}

int Market::getTotalItemsSold() const {
    int total = 0;
    for (const auto& [_, qty] : totalSellTransactions) {
        total += qty;
    }
    return total;
}

int Market::getTotalItemsBought() const {
    int total = 0;
    for (const auto& [_, qty] : totalBuyTransactions) {
        total += qty;
    }
    return total;
}


void Market::randomizePrices() {
    for (auto& [item, price] : prices) {
        price = 1 + rand() % 50;
    }
}


bool Market::sellItem(NPCEntity& npc, const std::string& item, int quantity) {
    if (item.empty() || quantity <= 0) return false;
    if (prices.find(item) == prices.end()) setPrice(item, 1 + std::rand() % 50);

    int inventoryCount = npc.getInventoryItemCount(item);
    if (inventoryCount < quantity) {
        getDebugConsole().log("ERROR", npc.getName() + " tried to sell more than they have.");
        return false;
    }

    if (!npc.removeFromInventory(item, quantity)) {
        getDebugConsole().log("ERROR", "Failed to remove " + std::to_string(quantity) + " " + item);
        return false;
    }

    float revenue = calculateSellPrice(item) * quantity;
    npc.setMoney(npc.getMoney() + revenue);
    MoneyManager::recordMoneyEarned(revenue);  // ✅ Track money earned

    supply[item] += quantity;
    demand[item] = std::max(0, demand[item] - quantity);
    prices[item] = adjustPriceOnSell(prices[item], demand[item], supply[item], priceAdjustmentFactor);

    totalSellTransactions[item] += quantity;
    totalRevenue[item] += revenue;

    npc.addReward(10.0f * quantity);

    getDebugConsole().log("MARKET", "[SELL] " + npc.getName() + " sold " + std::to_string(quantity) + " " + item);
    return true;
}


// Adjust price after a buy
float Market::adjustPriceOnBuy(float currentPrice, int demand, int supply, float buyFactor) {
    if (supply == 0) supply = 1;
    
    float changeFactor = buyFactor * (static_cast<float>(demand) / (supply + 10));  
    float newPrice = currentPrice * (1.0f + (changeFactor - 0.05f)); 

    return std::clamp(newPrice, minimumPrice, maximumPrice);
}

float Market::adjustPriceOnSell(float currentPrice, int demand, int supply, float sellFactor) {
    if (demand == 0) demand = 1;
    
    float changeFactor = -sellFactor * (static_cast<float>(supply) / (demand + 10));  
    float newPrice = currentPrice * (1.0f + (changeFactor + 0.05f)); 

    return std::clamp(newPrice, minimumPrice, maximumPrice);
}

std::string Market::suggestBestResourceToBuy() const {
    if (prices.empty()) return "wood"; // Default to wood if somehow empty.

    std::vector<std::string> essentialItems = {"wood", "stone", "bush"};
    std::string bestItem = "";
    float minPrice = std::numeric_limits<float>::max();

    for (const auto& item : essentialItems) {
        if (prices.find(item) != prices.end() && demand.at(item) > 5) { // Buy if in demand
            float price = calculateBuyPrice(item);
            if (price < minPrice) {
                minPrice = price;
                bestItem = item;
            }
        }
    }

    return bestItem.empty() ? "wood" : bestItem;  // Still defaults if no items qualify
}


std::string Market::suggestBestResourceToSell() const {
    if (prices.empty() || supply.empty()) {
        getDebugConsole().log("ERROR", "Market::suggestBestResourceToSell() - Prices or supply list is EMPTY.");
        return "";
    }

    std::vector<std::string> essentialItems = {"wood", "stone", "bush"};
    std::string bestItem = "";
    float highestPrice = 0.0f;

    for (const auto& item : essentialItems) {
        auto supplyIt = supply.find(item);
        if (supplyIt != supply.end() && supplyIt->second > 0) {
            float sellPrice = calculateSellPrice(item);
            if (sellPrice > highestPrice) {
                highestPrice = sellPrice;
                bestItem = item;
            }
        }
    }

    return bestItem;
}



// Simulate market dynamics
void Market::simulateMarketDynamics(float deltaTime) {
    static float timeAccumulator = 0.0f;
    timeAccumulator += deltaTime;

    // ✅ Only update prices every 10-15 seconds
    if (timeAccumulator < 2.0f) return;  
    timeAccumulator = 0.0f;  // Reset timer

    for (auto& [item, price] : prices) {
        int oldDemand = demand[item];
        int oldSupply = supply[item];

        // Simulate natural market fluctuations, but **slowly**
        demand[item] = std::max(10, demand[item] + (rand() % 3 - 1));  
        supply[item] = std::max(10, supply[item] + (rand() % 3 - 1));  

        // Calculate a **smaller** price change based on demand/supply
        float demandFactor = 1.0f + ((demand[item] - oldDemand) / 500.0f);  // Reduced impact
        float supplyFactor = 1.0f - ((supply[item] - oldSupply) / 500.0f);  // Reduced impact

        float newPrice = price * demandFactor * supplyFactor;

        // ✅ Ensure stability: Prices won’t swing too fast
        prices[item] = std::clamp(newPrice, minimumPrice, maximumPrice);

        // Track price history for UI graph
        trackPriceHistory(item);
    }
}



// Track price history
void Market::trackPriceHistory(const std::string& item) {
    priceHistory[item].push_back(prices[item]);
    if (priceHistory[item].size() > 10) {
        priceHistory[item].erase(priceHistory[item].begin());
    }
}

// Calculate volatility
float Market::calculateVolatility(const std::string& item) const {
    auto it = priceHistory.find(item);
    if (it == priceHistory.end() || it->second.size() < 2) return 0.0f;

    float mean = std::accumulate(it->second.begin(), it->second.end(), 0.0f) / it->second.size();
    float variance = 0.0f;
    for (float price : it->second) {
        variance += std::pow(price - mean, 2);
    }
    float volatility = std::sqrt(variance / it->second.size());

    getDebugConsole().log("MARKET", "[VOLATILITY UPDATE] " + item + " = " + std::to_string(volatility));
    return volatility;
}


// Display market prices
void Market::displayPrices() const {
    for (const auto& [item, price] : prices) {
        getDebugConsole().log("Market", "- " + item + ": $" + std::to_string(price));
        getDebugConsole().log("Market", "  Buy: " + std::to_string(getBuyTransactions(item)));
        getDebugConsole().log("Market", "  Sell: " + std::to_string(getSellTransactions(item)));
        getDebugConsole().log("Market", "  Revenue: $" + std::to_string(getRevenue(item)));
        getDebugConsole().log("Market", "  Expenditure: $" + std::to_string(getExpenditure(item)));
    }
}


// Stabilize prices
void Market::stabilizePrices(float deltaTime) {
    for (auto& [item, price] : prices) {
        float ratio = static_cast<float>(demand[item]) / (supply[item] + 1);
        float targetPrice = 10.0f * (1.0f + (ratio - 1.0f) * 0.05f);
        price += (targetPrice - price) * deltaTime * 0.1f;
        trackPriceHistory(item);
    }
}

// Render price graph
void Market::renderPriceGraph(sf::RenderWindow& window, const std::string& item, sf::Vector2f position, sf::Vector2f size) const {
    auto it = priceHistory.find(item);
    if (it == priceHistory.end() || it->second.empty()) return;

    const auto& history = it->second;
    sf::VertexArray graph(sf::LineStrip, history.size());

    float maxPrice = *std::max_element(history.begin(), history.end());
    float minPrice = *std::min_element(history.begin(), history.end());

    for (size_t i = 0; i < history.size(); ++i) {
        float normalized = (history[i] - minPrice) / (maxPrice - minPrice);
        graph[i].position = {position.x + i * (size.x / history.size()), position.y + size.y - normalized * size.y};
        graph[i].color = sf::Color::Green;
    }

    window.draw(graph);
}

// Draw the market
void Market::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Get object type
ObjectType Market::getType() const {
    return ObjectType::Market;
}

// Get resource stats
std::unordered_map<std::string, float> Market::getResourceStats() const {
    std::unordered_map<std::string, float> stats;
    for (const auto& [item, price] : prices) {
        stats[item] = price;
    }
    return stats;
}


// Get total trades
int Market::getTotalTrades() const {
    int total = 0;
    for (const auto& [_, qty] : supply) {
        total += qty;
    }
    return total;
}

// Get buy transactions
int Market::getBuyTransactions(const std::string& item) const {
    auto it = totalBuyTransactions.find(item);
    return it != totalBuyTransactions.end() ? it->second : 0;
}

// Get sell transactions
int Market::getSellTransactions(const std::string& item) const {
    auto it = totalSellTransactions.find(item);
    return it != totalSellTransactions.end() ? it->second : 0;
}

// Get revenue
float Market::getRevenue(const std::string& item) const {
    auto it = totalRevenue.find(item);
    return it != totalRevenue.end() ? it->second : 0.0f;
}

// Get expenditure
float Market::getExpenditure(const std::string& item) const {
    auto it = totalExpenditure.find(item);
    return it != totalExpenditure.end() ? it->second : 0.0f;
}

// Get all prices
const std::unordered_map<std::string, float>& Market::getPrices() const {
    return prices;
}

// Get price trend map
const std::unordered_map<std::string, std::vector<float>>& Market::getPriceTrendMap() const {
    return priceHistory;
}
