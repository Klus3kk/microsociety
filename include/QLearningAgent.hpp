#ifndef QLEARNING_AGENT_HPP
#define QLEARNING_AGENT_HPP

#include "State.hpp"
#include "Tile.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <ActionType.hpp>
class QLearningAgent {
private:
    float learningRate;    // Learning rate (alpha)
    float discountFactor;  // Discount factor (gamma)
    float epsilon;         // Exploration rate
    std::unordered_map<State, std::unordered_map<ActionType, float>, StateHasher> QTable;

public:
    QLearningAgent(float learningRate, float discountFactor, float epsilon);

    ActionType decideAction(const State& state); // Choose an action based on Q-table
    void updateQValue(const State& state, ActionType action, float reward, const State& nextState);

    // Helpers for state extraction
    State extractState(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                       const sf::Vector2f& position, float energy, int inventorySize, int maxInventorySize) const;

    static int quantize(float value, float minValue, float maxValue, int levels);
    static int countNearbyObjects(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                                  const sf::Vector2f& position, ObjectType objectType);
};

#endif // QLEARNING_AGENT_HPP
