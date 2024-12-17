#include "NPCEntity.hpp"
#include "Actions.hpp" // Include full definition of Action
#include <algorithm>
#include <numeric>

// Constructor
NPCEntity::NPCEntity(const std::string& npcName, float initHealth, float initHunger, float initEnergy,
                     float initSpeed, float initStrength, float initMoney)
    : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney), 
      name(npcName) {}

// Getters
const std::string& NPCEntity::getName() const { return name; }
float NPCEntity::getMaxEnergy() const { return MAX_ENERGY; }
float NPCEntity::getBaseSpeed() const { return baseSpeed; }
float NPCEntity::getEnergyPercentage() const { return energy / MAX_ENERGY; }
const std::unordered_map<std::string, int>& NPCEntity::getInventory() const { return inventory; }
int NPCEntity::getMaxInventorySize() const { return inventoryCapacity; }
int NPCEntity::getInventorySize() const {
    return std::accumulate(inventory.begin(), inventory.end(), 0,
                           [](int total, const auto& pair) { return total + pair.second; });
}

// Inventory Management
bool NPCEntity::addToInventory(const std::string& item, int quantity) {
    if (getInventorySize() + quantity > inventoryCapacity) {
        getDebugConsole().logOnce("Inventory", name + "'s inventory full! Cannot add " + item + ".");
        return false;
    }
    inventory[item] += quantity;
    getDebugConsole().log("Inventory", name + " added " + std::to_string(quantity) + " " + item + "(s) to inventory.");
    return true;
}

bool NPCEntity::removeFromInventory(const std::string& item, int quantity) {
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

int NPCEntity::getInventoryItemCount(const std::string& item) const {
    auto it = inventory.find(item);
    return (it != inventory.end()) ? it->second : 0;
}

// Reward and Penalty Management
void NPCEntity::addReward(int reward) {
    currentReward += reward;
    getDebugConsole().log(name, name + " received a reward of " + std::to_string(reward) + ".");
}

void NPCEntity::addPenalty(int penalty) {
    currentPenalty += penalty;
    getDebugConsole().log(name, name + " incurred a penalty of " + std::to_string(penalty) + ".");
}

// Energy Management
void NPCEntity::consumeEnergy(float amount) {
    energy = std::max(0.0f, energy - amount);
    if (energy == 0.0f) {
        getDebugConsole().log(name, name + " has no energy and is marked for death!");
    }
}

void NPCEntity::regenerateEnergy(float rate) {
    if (energy < MAX_ENERGY) {
        energy = std::min(MAX_ENERGY, energy + rate);
        getDebugConsole().log(name, name + "'s energy regenerated to " + std::to_string(energy));
    }
}
// Perform Action
void NPCEntity::performAction(std::unique_ptr<Action> action, Tile& tile) {
    if (action) {
        action->perform(*this, tile);
    }
}

// Handle NPC Death
bool NPCEntity::isDead() const {
    return energy == 0.0f || health == 0.0f;
}

void NPCEntity::handleDeath() {
    getDebugConsole().log(name, name + " has died.");
    addPenalty(deathPenalty);
}

// Inventory Capacity Upgrades
void NPCEntity::upgradeInventoryCapacity(int extraSlots) {
    inventoryCapacity += extraSlots;
    getDebugConsole().log(name, name + "'s inventory capacity upgraded to " + std::to_string(inventoryCapacity));
}

void NPCEntity::setHealth(float newHealth) {
    health = std::clamp(newHealth, 0.0f, MAX_HEALTH);
    if (health == 0.0f) {
        setDead(true);
    }
}

void NPCEntity::setStrength(float newStrength) { strength = newStrength; }
void NPCEntity::setSpeed(float newSpeed) { speed = newSpeed; }

void NPCEntity::update(float deltaTime) {
    // Ensure cooldown decrements properly
    if (currentActionCooldown > 0) {
        currentActionCooldown -= deltaTime;
        currentActionCooldown = std::max(0.0f, currentActionCooldown); // Clamp to 0
    }

    if (energy > 0) {
        energy = std::max(0.0f, energy - deltaTime * 0.5f); // Energy decay
    }

    if (isDead()) {
        handleDeath();
    }

    getDebugConsole().log(name, "Updated NPC state for " + name);
}


// Brain of the AI
ActionType NPCEntity::decideNextAction(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Debug log cooldown
    getDebugConsole().log("AI Cooldown", getName() + " cooldown: " + std::to_string(currentActionCooldown));

    if (currentActionCooldown > 0) {
        return ActionType::Idle; // Skip actions during cooldown
    }

    auto nearbyObjects = scanNearbyTiles(tileMap);

    for (ObjectType obj : nearbyObjects) {
        getDebugConsole().log("AI Decision", getName() + " sees nearby object of type: " + std::to_string(static_cast<int>(obj)));
    }

    if (getEnergy() < 20.0f) {
        currentActionCooldown = actionCooldownTime;
        return ActionType::RegenerateEnergy;
    }
    if (getHunger() < 30.0f) {
        currentActionCooldown = actionCooldownTime;
        return ActionType::GatherBush;
    }

    for (ObjectType objType : nearbyObjects) {
        if (objType == ObjectType::Tree) {
            currentActionCooldown = actionCooldownTime;
            return ActionType::ChopTree;
        }
        if (objType == ObjectType::Rock) {
            currentActionCooldown = actionCooldownTime;
            return ActionType::MineRock;
        }
        if (objType == ObjectType::Bush) {
            currentActionCooldown = actionCooldownTime;
            return ActionType::GatherBush;
        }
    }

    if (getInventorySize() >= getMaxInventorySize()) {
        currentActionCooldown = actionCooldownTime;
        return ActionType::StoreItem;
    }

    currentActionCooldown = actionCooldownTime;
    return ActionType::Explore;
}


std::vector<ObjectType> NPCEntity::scanNearbyTiles(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) const {
    std::vector<ObjectType> nearbyObjects;

    // Scan surrounding tiles
    int npcX = static_cast<int>(getPosition().x / GameConfig::tileSize);
    int npcY = static_cast<int>(getPosition().y / GameConfig::tileSize);

    for (int y = npcY - 1; y <= npcY + 1; ++y) {
        for (int x = npcX - 1; x <= npcX + 1; ++x) {
            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                if (tileMap[y][x]->hasObject()) {
                    nearbyObjects.push_back(tileMap[y][x]->getObject()->getType());
                }
            }
        }
    }

    return nearbyObjects;
}
