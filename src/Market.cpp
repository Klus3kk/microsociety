#include "Market.hpp"
#include "NPCEntity.hpp"
#include "PlayerEntity.hpp"
#include "MoneyManager.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <ctime>
#include <cstdlib>

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

// FIXED: Changed parameter from NPCEntity& to Entity&
bool Market::buyItem(Entity& entity, const std::string& item, int quantity) {
    if (quantity <= 0 || item.empty()) return false;
    if (prices.find(item) == prices.end()) setPrice(item, 1 + std::rand() % 50);

    float itemPrice = calculateBuyPrice(item);
    float totalCost = itemPrice * quantity;

    if (entity.getMoney() < totalCost) {  
        getDebugConsole().log("MARKET", "Entity cannot afford " + std::to_string(quantity) + " " + item + 
                            " (needs $" + std::to_string(totalCost) + ", has $" + std::to_string(entity.getMoney()) + ")");
        return false;
    }

    // Check if entity can add to inventory
    bool inventorySuccess = false;
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        inventorySuccess = npc->addToInventory(item, quantity);
    } else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
        inventorySuccess = player->addToInventory(item, quantity);
    }
    
    if (!inventorySuccess) {
        getDebugConsole().log("MARKET", "Entity inventory FULL. Cannot buy " + item);
        return false;
    }

    // Process the transaction
    entity.setMoney(entity.getMoney() - totalCost);
    MoneyManager::recordMoneySpent(totalCost);

    // FIXED: Update market state properly
    demand[item] += quantity;
    supply[item] = std::max(0, supply[item] - quantity);
    prices[item] = adjustPriceOnBuy(prices[item], demand[item], supply[item], priceAdjustmentFactor);

    // FIXED: Ensure transaction tracking works
    totalBuyTransactions[item] += quantity;
    totalExpenditure[item] += totalCost;

    // Add reward for NPCs
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        npc->addReward(5.0f * quantity);
    }

    // FIXED: Log successful transaction with details
    getDebugConsole().log("MARKET", "[BUY SUCCESS] " + 
                        (dynamic_cast<NPCEntity*>(&entity) ? 
                         dynamic_cast<NPCEntity*>(&entity)->getName() : "Player") + 
                        " bought " + std::to_string(quantity) + " " + item + 
                        " for $" + std::to_string(totalCost) + 
                        " (total buys: " + std::to_string(totalBuyTransactions[item]) + ")");
    
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

// FIXED: Changed parameter from NPCEntity& to Entity&
bool Market::sellItem(Entity& entity, const std::string& item, int quantity) {
    if (item.empty() || quantity <= 0) return false;
    if (prices.find(item) == prices.end()) setPrice(item, 1 + std::rand() % 50);

    int inventoryCount = 0;
    
    // Check inventory count based on entity type
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        inventoryCount = npc->getInventoryItemCount(item);
    } else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
        inventoryCount = player->getInventoryItemCount(item);
    } else {
        getDebugConsole().log("MARKET", "Unknown entity type trying to sell items");
        return false;
    }

    if (inventoryCount < quantity) {
        getDebugConsole().log("MARKET", "Entity tried to sell " + std::to_string(quantity) + 
                            " " + item + " but only has " + std::to_string(inventoryCount));
        return false;
    }

    // Remove from inventory based on entity type
    bool removeSuccess = false;
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        removeSuccess = npc->removeFromInventory(item, quantity);
    } else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
        removeSuccess = player->removeFromInventory(item, quantity);
    }

    if (!removeSuccess) {
        getDebugConsole().log("MARKET", "Failed to remove " + std::to_string(quantity) + " " + item + " from inventory");
        return false;
    }

    float revenue = calculateSellPrice(item) * quantity;
    entity.setMoney(entity.getMoney() + revenue);
    MoneyManager::recordMoneyEarned(revenue);

    // FIXED: Update market state properly
    supply[item] += quantity;
    demand[item] = std::max(0, demand[item] - quantity);
    prices[item] = adjustPriceOnSell(prices[item], demand[item], supply[item], priceAdjustmentFactor);

    // FIXED: Ensure transaction tracking works
    totalSellTransactions[item] += quantity;
    totalRevenue[item] += revenue;

    // Add reward for NPCs
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        npc->addReward(10.0f * quantity);
    }

    // FIXED: Log successful transaction with details
    getDebugConsole().log("MARKET", "[SELL SUCCESS] " + 
                        (dynamic_cast<NPCEntity*>(&entity) ? 
                         dynamic_cast<NPCEntity*>(&entity)->getName() : "Player") + 
                        " sold " + std::to_string(quantity) + " " + item + 
                        " for $" + std::to_string(revenue) + 
                        " (total sells: " + std::to_string(totalSellTransactions[item]) + ")");

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

        // ✅ Ensure stability: Prices won't swing too fast
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

void Market::debugTransactionState() const {
    getDebugConsole().log("MARKET_DEBUG", "=== Market Transaction State ===");
    
    for (const auto& [item, price] : prices) {
        int buyCount = getBuyTransactions(item);
        int sellCount = getSellTransactions(item);
        float revenue = getRevenue(item);
        float expenditure = getExpenditure(item);
        
        getDebugConsole().log("MARKET_DEBUG", item + ": Price=$" + std::to_string(price) + 
                            ", Buys=" + std::to_string(buyCount) + 
                            ", Sells=" + std::to_string(sellCount) + 
                            ", Revenue=$" + std::to_string(revenue) + 
                            ", Expenditure=$" + std::to_string(expenditure));
    }
    
    getDebugConsole().log("MARKET_DEBUG", "Total items bought: " + std::to_string(getTotalItemsBought()));
    getDebugConsole().log("MARKET_DEBUG", "Total items sold: " + std::to_string(getTotalItemsSold()));
}

// Get all prices
const std::unordered_map<std::string, float>& Market::getPrices() const {
    return prices;
}

// Get price trend map
const std::unordered_map<std::string, std::vector<float>>& Market::getPriceTrendMap() const {
    return priceHistory;
}

