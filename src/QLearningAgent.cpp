#include "QLearningAgent.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <Configuration.hpp>

QLearningAgent::QLearningAgent(float learningRate, float discountFactor, float epsilon)
    : learningRate(learningRate), discountFactor(discountFactor), epsilon(epsilon), 
      rng(std::random_device{}()), actionDist(1, static_cast<int>(ActionType::Rest)) {}

ActionType QLearningAgent::decideAction(const State& state) {
    std::uniform_real_distribution<> explorationDist(0.0, 1.0);

    // Check if state exists in QTable
    auto it = QTable.find(state);
    if (explorationDist(rng) < epsilon || it == QTable.end()) {
        return static_cast<ActionType>(actionDist(rng));
    }

    // Exploit: Choose the action with the highest Q-value
    const auto& actions = it->second;
    return std::max_element(actions.begin(), actions.end(),
                            [](const auto& a, const auto& b) { return a.second < b.second; })->first;
}

void QLearningAgent::updateQValue(const State& state, ActionType action, float reward, const State& nextState) {
    float currentQ = QTable[state][action];
    float maxNextQ = 0.0f;

    // Get the best Q-value for next state
    auto it = QTable.find(nextState);
    if (it != QTable.end() && !it->second.empty()) {
        maxNextQ = std::max_element(it->second.begin(), it->second.end(),
                                    [](const auto& a, const auto& b) { return a.second < b.second; })->second;
    }

    // Adjust learning update
    float penaltyFactor = (reward < 0) ? 1.25f : 1.0f;
    float qUpdate = reward + (discountFactor * maxNextQ) - currentQ;
    
    QTable[state][action] += learningRate * penaltyFactor * qUpdate;
}

int QLearningAgent::quantize(float value, float minValue, float maxValue, int levels) {
    return std::clamp(static_cast<int>(std::round((value - minValue) * (levels - 1) / (maxValue - minValue))), 0, levels - 1);
}

int QLearningAgent::countNearbyObjects(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                                       const sf::Vector2f& position, ObjectType objectType) {
    int count = 0;
    int npcX = static_cast<int>(position.x / GameConfig::tileSize);
    int npcY = static_cast<int>(position.y / GameConfig::tileSize);

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int x = npcX + dx;
            int y = npcY + dy;

            if (x < 0 || y < 0 || x >= tileMap[0].size() || y >= tileMap.size()) continue;

            auto& tile = tileMap[y][x];
            if (tile && tile->hasObject() && tile->getObject()->getType() == objectType) {
                count++;
            }
        }
    }

    return count;
}

State QLearningAgent::extractState(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                                   const sf::Vector2f& position, float energy, int inventorySize, int maxInventorySize) const {
    State state;

    constexpr float INV_TILE_SIZE = 1.0f / GameConfig::tileSize;
    int posX = static_cast<int>(position.x * INV_TILE_SIZE);
    int posY = static_cast<int>(position.y * INV_TILE_SIZE);
    state.posX = posX;
    state.posY = posY;

    state.nearbyTrees = 0;
    state.nearbyRocks = 0;
    state.nearbyBushes = 0;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int x = posX + dx, y = posY + dy;
            if (x < 0 || y < 0 || x >= tileMap[0].size() || y >= tileMap.size()) continue;

            if (auto& tile = tileMap[y][x]; tile && tile->hasObject()) {
                switch (tile->getObject()->getType()) {
                    case ObjectType::Tree: state.nearbyTrees++; break;
                    case ObjectType::Rock: state.nearbyRocks++; break;
                    case ObjectType::Bush: state.nearbyBushes++; break;
                    default: break;
                }
            }
        }
    }

    constexpr int ENERGY_LEVELS = 3;
    constexpr int INVENTORY_LEVELS = 3;
    state.energyLevel = quantize(energy, 0, 100, ENERGY_LEVELS);
    state.inventoryLevel = quantize(inventorySize, 0, maxInventorySize, INVENTORY_LEVELS);

    return state;
}
