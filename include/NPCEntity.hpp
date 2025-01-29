#ifndef NPC_HPP
#define NPC_HPP

#include "Entity.hpp"
#include "debug.hpp"
#include "Tile.hpp"
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <memory>
#include "ActionType.hpp"
#include "QLearningAgent.hpp"

class Action; // Forward declaration of Action class
class Market;
class House;

enum class NPCState {
    Idle,
    Walking,
    PerformingAction,
    EvaluatingState
};

class NPCEntity : public Entity {
private:
    static constexpr float MAX_HEALTH = 100.0f;
    static constexpr float MAX_ENERGY = 100.0f;
    NPCState currentState = NPCState::Idle; 
    Tile* target = nullptr;
    ActionType currentAction = ActionType::None; 
    QLearningAgent agent; // Q-learning agent for decision-making
    bool useQLearning = false; // Toggle Q-learning behavior
    std::unordered_map<std::string, int> inventory; // Map for items and their quantities
    int inventoryCapacity = 10;                     // Max inventory capacity
    std::string name;                               // NPC's name
    float baseSpeed = 150.0f;                       // Default base speed
    float currentSpeed = baseSpeed;                 // Current movement speed
    int deathPenalty = -100;                        // Penalty for NPC death
    int currentReward = 0;                          // Reward balance
    int currentPenalty = 0;                         // Penalty balance
    float currentActionCooldown = 0.0f;             // Time remaining before next action
    const float actionCooldownTime = 2.0f;          // Time between actions (adjust as needed)
    House* house;
    ActionType lastAction;                          // Last action performed by NPC
    State currentQLearningState;  

public:
    // Constructor
    NPCEntity(const std::string& npcName, float initHealth, float initHunger, float initEnergy,
              float initSpeed, float initStrength, float initMoney, bool enableQLearning = false);

    // Getters
    const std::string& getName() const;
    float getMaxEnergy() const;
    float getBaseSpeed() const;
    float getEnergyPercentage() const;
    const std::unordered_map<std::string, int>& getInventory() const;
    int getMaxInventorySize() const;
    int getInventorySize() const;
    // Modify getter to return a reference
    float& getMoney(); // For modifiable access
    const float& getMoney() const; // For read-only access
    void setTarget(Tile* newTarget);
    void setHouse(House* assignedHouse); // Setter for house
    House* getHouse();

    bool isAtTarget() const;
    // Inventory Management
    bool addToInventory(const std::string& item, int quantity);
    bool removeFromInventory(const std::string& item, int quantity);
    int getInventoryItemCount(const std::string& item) const;

    // Reward and Penalty Management
    void addReward(int reward);
    void addPenalty(int penalty);

    // Energy Management
    void consumeEnergy(float amount);
    void regenerateEnergy(float rate);

    // Decision-Making Hook (AI will override this)
    ActionType decideNextAction(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, const House& house, Market& market);
    std::vector<ObjectType> scanNearbyTiles(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) const;
    Tile* findNearestTile(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, ObjectType type) const;
    Tile* getTarget() const; // Getter for the target tile
    // Perform Action
    void performAction(ActionType action, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, Market& market, House& house);
    void update(float deltaTime);

    // Handle NPC Death
    bool isDead() const;
    void handleDeath();

    // Inventory Capacity Upgrades
    void upgradeInventoryCapacity(int extraSlots);
    void setHealth(float newHealth);
    void setStrength(float newStrength);
    void setSpeed(float newSpeed);

    // Q-learning integration
    int countNearbyObjects(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, ObjectType type) const;
    void receiveFeedback(float reward, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap); // Update Q-table after action
    void enableQLearning(bool enable); // Toggle Q-learning behavior
    State extractState(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) const; // State representation
    void updateQLearningState(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap);
    // State management
    void setState(NPCState newState) { currentState = newState; }
    NPCState getState() const { return currentState; }

    // Current action accessors
    void setCurrentAction(ActionType action) { currentAction = action; }
    ActionType getCurrentAction() const { return currentAction; }
};

#endif
