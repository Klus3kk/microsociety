#ifndef NPC_HPP
#define NPC_HPP

#include "Entity.hpp"
#include "debug.hpp"
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include "Actions.hpp"

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
              float initSpeed, float initStrength, float initMoney)
        : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney), 
          name(npcName) {}

    // Getters
    const std::string& getName() const { return name; }
    float getMaxEnergy() const { return MAX_ENERGY; }
    float getBaseSpeed() const { return baseSpeed; }
    float getEnergyPercentage() const { return energy / MAX_ENERGY; }
    const std::unordered_map<std::string, int>& getInventory() const { return inventory; }
    int getMaxInventorySize() const { return inventoryCapacity; }
    int getInventorySize() const {
        return std::accumulate(inventory.begin(), inventory.end(), 0,
                               [](int total, const auto& pair) { return total + pair.second; });
    }

    // Inventory Management
    bool addToInventory(const std::string& item, int quantity) {
        if (getInventorySize() + quantity > inventoryCapacity) {
            getDebugConsole().logOnce("Inventory", name + "'s inventory full! Cannot add " + item + ".");
            return false;
        }
        inventory[item] += quantity;
        getDebugConsole().log("Inventory", name + " added " + std::to_string(quantity) + " " + item + "(s) to inventory.");
        return true;
    }

    bool removeFromInventory(const std::string& item, int quantity) {
        auto it = inventory.find(item);
        if (it != inventory.end() && it->second >= quantity) {
            it->second -= quantity;
            if (it->second == 0) inventory.erase(it);
            getDebugConsole().log("Inventory", name + " removed " + std::to_string(quantity) + " " + item + "(s) from inventory.");
            return true;
        }
        getDebugConsole().logOnce("Inventory", name + " failed to remove " + std::to_string(quantity) + " " + item + ".");
        return false;
    }

    int getInventoryItemCount(const std::string& item) const {
        auto it = inventory.find(item);
        return (it != inventory.end()) ? it->second : 0;
    }

    // Reward and Penalty Management
    void addReward(int reward) {
        currentReward += reward;
        getDebugConsole().log(name, name + " received a reward of " + std::to_string(reward) + ".");
    }

    void addPenalty(int penalty) {
        currentPenalty += penalty;
        getDebugConsole().log(name, name + " incurred a penalty of " + std::to_string(penalty) + ".");
    }

    // Energy Management
    void consumeEnergy(float amount) {
        energy = std::max(0.0f, energy - amount);
        if (energy == 0.0f) {
            getDebugConsole().log(name, name + " has no energy and is marked for death!");
        }
    }

    void regenerateEnergy(float rate) {
        if (energy < MAX_ENERGY) {
            energy = std::min(MAX_ENERGY, energy + rate);
            getDebugConsole().log(name, name + "'s energy regenerated to " + std::to_string(energy));
        }
    }

    // Decision-Making Hook (AI will override this)
    ActionType decideNextAction() {
        // Placeholder: Use AI logic to decide action based on state
        return ActionType::None;
    }

    // Perform Action
    void performAction(Action& action, Tile& tile) {
        action.perform(*this, tile);
    }

    // Handle NPC Death
    bool isDead() const {
        return energy == 0.0f || health == 0.0f;
    }

    void handleDeath() {
        getDebugConsole().log(name, name + " has died.");
        // Apply death penalty or other logic
        addPenalty(deathPenalty);
    }

    // Inventory Capacity Upgrades
    void upgradeInventoryCapacity(int extraSlots) {
        inventoryCapacity += extraSlots;
        getDebugConsole().log(name, name + "'s inventory capacity upgraded to " + std::to_string(inventoryCapacity));
    }
    void setHealth(float newHealth) {
        health = std::clamp(newHealth, 0.0f, MAX_HEALTH);
        if (health == 0.0f) {
            setDead(true);
        }
    }

    void setStrength(float newStrength) { strength = newStrength; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
};

#endif
