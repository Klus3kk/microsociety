#include "Market.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include "House.hpp"
// Default constructor
Market::Market() {
    sf::Texture defaultTexture;
    texture = defaultTexture;
    sprite.setTexture(texture);

    setPrice("wood", 10.0f);
    setPrice("stone", 10.0f);
    setPrice("bush", 5.0f);

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

// Player buys an item
bool Market::buyItem(NPCEntity& npc, const std::string& item, int quantity) {
    if (quantity <= 0 || item.empty()) return false;
    if (prices.find(item) == prices.end()) setPrice(item, 10.0f);

    float itemPrice = calculateBuyPrice(item);
    float totalCost = itemPrice * quantity;

    if (npc.getMoney() < totalCost) {  // Ensure NPC can afford purchase
        getDebugConsole().log("ERROR", npc.getName() + " cannot afford " + std::to_string(quantity) + " " + item);
        return false;
    }

    if (!npc.addToInventory(item, quantity)) {
        getDebugConsole().log("ERROR", npc.getName() + " inventory FULL. Cannot buy " + item);
        return false;
    }

    npc.setMoney(npc.getMoney() - totalCost);
    demand[item] += quantity;
    supply[item] = std::max(0, supply[item] - quantity);
    prices[item] = adjustPriceOnBuy(prices[item], demand[item], supply[item], priceAdjustmentFactor);

    totalBuyTransactions[item] += quantity;
    totalExpenditure[item] += totalCost;
    npc.addReward(5.0f * quantity);

    getDebugConsole().log("MARKET", npc.getName() + " successfully bought " + std::to_string(quantity) + " " + item);
    return true;
}





// Player sells an item
bool Market::sellItem(NPCEntity& npc, const std::string& item, int quantity) {
    if (item.empty() || quantity <= 0) return false;
    if (prices.find(item) == prices.end()) setPrice(item, 10.0f);

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
    supply[item] += quantity;
    demand[item] = std::max(0, demand[item] - quantity);
    prices[item] = adjustPriceOnSell(prices[item], demand[item], supply[item], priceAdjustmentFactor);

    totalSellTransactions[item] += quantity;
    totalRevenue[item] += revenue;
    npc.addReward(10.0f * quantity);

    getDebugConsole().log("MARKET", npc.getName() + " successfully sold " + std::to_string(quantity) + " " + item);
    return true;
}






// Adjust price after a buy
float Market::adjustPriceOnBuy(float currentPrice, int demand, int supply, float buyFactor) {
    if (supply == 0) supply = 1;
    float adjustmentFactor = buyFactor * (static_cast<float>(demand) / supply);
    return std::clamp(currentPrice * (1.0f + adjustmentFactor), minimumPrice, currentPrice * 2.0f);
}



// Adjust price after a sell
float Market::adjustPriceOnSell(float currentPrice, int demand, int supply, float sellFactor) {
    if (demand == 0) demand = 1;
    float adjustmentFactor = -sellFactor * (static_cast<float>(supply) / demand);
    return std::clamp(currentPrice * (1.0f + adjustmentFactor), minimumPrice, currentPrice * 2.0f);
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
    for (auto& [item, price] : prices) {
        // Simulate demand and supply changes
        demand[item] = std::max(10, demand[item] + static_cast<int>(std::sin(deltaTime) * 5));
        supply[item] = std::max(10, supply[item] - static_cast<int>(std::cos(deltaTime) * 5));

        // Stabilize prices based on new dynamics
        stabilizePrices(deltaTime);
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
    return std::sqrt(variance / it->second.size());
}

// Display market prices
void Market::displayPrices() const {
    for (const auto& [item, price] : prices) {
        getDebugConsole().log("Market", "- " + item + ": $" + std::to_string(price));
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
