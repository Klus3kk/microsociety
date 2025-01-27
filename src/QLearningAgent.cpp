#include "QLearningAgent.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <Configuration.hpp>
// Constructor
QLearningAgent::QLearningAgent(float learningRate, float discountFactor, float epsilon)
    : learningRate(learningRate), discountFactor(discountFactor), epsilon(epsilon) {}

// Decide the next action using epsilon-greedy policy
ActionType QLearningAgent::decideAction(const State& state) {
    // Epsilon-greedy decision-making
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    if (dist(gen) < epsilon) {
        // Explore: Random action
        return static_cast<ActionType>(rand() % static_cast<int>(ActionType::Idle) + 1);
    } else {
        // Exploit: Choose the action with the highest Q-value
        if (QTable.find(state) == QTable.end()) {
            return static_cast<ActionType>(rand() % static_cast<int>(ActionType::Idle) + 1);
        }

        auto& actions = QTable[state];
        return std::max_element(actions.begin(), actions.end(),
                                [](const auto& a, const auto& b) { return a.second < b.second; })
            ->first;
    }
}

// Update Q-value
void QLearningAgent::updateQValue(const State& state, ActionType action, float reward, const State& nextState) {
    float currentQ = QTable[state][action];
    float maxNextQ = 0.0f;

    if (QTable.find(nextState) != QTable.end()) {
        auto& actions = QTable[nextState];
        maxNextQ = std::max_element(actions.begin(), actions.end(),
                                    [](const auto& a, const auto& b) { return a.second < b.second; })
                       ->second;
    }

    QTable[state][action] = currentQ + learningRate * (reward + discountFactor * maxNextQ - currentQ);
}

// Extract state from the environment
State QLearningAgent::extractState(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                                   const sf::Vector2f& position, float energy, int inventorySize, int maxInventorySize) const {
    State state;

    // Determine NPC's position on the grid
    int posX = static_cast<int>(position.x / GameConfig::tileSize);
    int posY = static_cast<int>(position.y / GameConfig::tileSize);

    // Count nearby objects
    state.posX = posX;
    state.posY = posY;
    state.nearbyTrees = countNearbyObjects(tileMap, position, ObjectType::Tree);
    state.nearbyRocks = countNearbyObjects(tileMap, position, ObjectType::Rock);
    state.nearbyBushes = countNearbyObjects(tileMap, position, ObjectType::Bush);

    // Quantize energy and inventory levels
    state.energyLevel = quantize(energy, 0, 100, 3);
    state.inventoryLevel = quantize(inventorySize, 0, maxInventorySize, 3);

    return state;
}

// Quantize a continuous value into discrete levels
int QLearningAgent::quantize(float value, float minValue, float maxValue, int levels) {
    value = std::clamp(value, minValue, maxValue);
    float levelSize = (maxValue - minValue) / levels;
    return static_cast<int>((value - minValue) / levelSize);
}


// Count nearby objects of a specific type
int QLearningAgent::countNearbyObjects(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                                       const sf::Vector2f& position, ObjectType objectType) {
    int count = 0;

    int npcX = static_cast<int>(position.x / GameConfig::tileSize);
    int npcY = static_cast<int>(position.y / GameConfig::tileSize);

    for (int y = npcY - 1; y <= npcY + 1; ++y) {
        for (int x = npcX - 1; x <= npcX + 1; ++x) {
            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                if (tileMap[y][x]->hasObject() && tileMap[y][x]->getObject()->getType() == objectType) {
                    count++;
                }
            }
        }
    }

    return count;
}
