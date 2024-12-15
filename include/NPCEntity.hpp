#ifndef NPC_HPP
#define NPC_HPP

#include "Entity.hpp"
#include "debug.hpp"
#include "Tile.hpp" // Include only for Tile reference
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <memory> // For std::unique_ptr
#include "ActionType.hpp"

class Action; // Forward declaration of Action class

class NPCEntity : public Entity {
private:
    static constexpr float MAX_HEALTH = 100.0f;
    static constexpr float MAX_ENERGY = 100.0f;

    std::unordered_map<std::string, int> inventory; // Map for items and their quantities
    int inventoryCapacity = 10;                     // Max inventory capacity
    std::string name;                               // NPC's name
    float baseSpeed = 150.0f;                       // Default base speed
    float currentSpeed = baseSpeed;                 // Current movement speed
    int deathPenalty = -100;                        // Penalty for NPC death
    int currentReward = 0;                          // Reward balance
    int currentPenalty = 0;                         // Penalty balance

public:
    // Constructor
    NPCEntity(const std::string& npcName, float initHealth, float initHunger, float initEnergy,
              float initSpeed, float initStrength, float initMoney);

    // Getters
    const std::string& getName() const;
    float getMaxEnergy() const;
    float getBaseSpeed() const;
    float getEnergyPercentage() const;
    const std::unordered_map<std::string, int>& getInventory() const;
    int getMaxInventorySize() const;
    int getInventorySize() const;

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
    ActionType decideNextAction(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap);
    std::vector<ObjectType> scanNearbyTiles(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) const;
    // Perform Action
    void performAction(std::unique_ptr<Action> action, Tile& tile); // No changes needed as it modifies the entity
    void update(float deltaTime); // Ensure this modifies the entity


    // Handle NPC Death
    bool isDead() const;
    void handleDeath();

    // Inventory Capacity Upgrades
    void upgradeInventoryCapacity(int extraSlots);
    void setHealth(float newHealth);
    void setStrength(float newStrength);
    void setSpeed(float newSpeed);
};

#endif
