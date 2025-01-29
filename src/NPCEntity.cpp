#include "NPCEntity.hpp"
#include "Actions.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

// Constructor
NPCEntity::NPCEntity(const std::string& npcName, float initHealth, float initHunger, float initEnergy,
                     float initSpeed, float initStrength, float initMoney, bool enableQLearning)
    : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney),
      agent(0.1f, 0.9f, 0.1f), // Initialize Q-learning agent
      useQLearning(enableQLearning), // Enable/disable Q-learning
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

void NPCEntity::updateQLearningState(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    currentQLearningState = extractState(tileMap);
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
    if (item.empty()) {
        getDebugConsole().log("ERROR", getName() + " removeFromInventory() received an EMPTY item name.");
        return false;
    }

    auto it = inventory.find(item);
    if (it == inventory.end()) {
        getDebugConsole().log("ERROR", getName() + " attempted to remove an item that DOES NOT EXIST: " + item);
        return false;
    }

    if (it->second < quantity) {
        getDebugConsole().log("ERROR", getName() + " tried to remove more items than they HAVE: " + item);
        return false;
    }

    it->second -= quantity;
    if (it->second <= 0) inventory.erase(it); // Remove if quantity reaches zero

    getDebugConsole().log("Inventory", getName() + " removed " + std::to_string(quantity) + " " + item);
    return true;
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
void NPCEntity::performAction(ActionType action, Tile& tile,
    const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
    Market& market, House& house) {

    std::unique_ptr<Action> actionPtr = nullptr;
    float actionReward = 0.0f;
    bool actionSuccess = false;

    switch (action) {
        case ActionType::ChopTree:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Tree) {
                actionPtr = std::make_unique<TreeAction>();
                actionReward = 10.0f;
                actionSuccess = true;
            } else {
                actionReward = -5.0f; // Penalty for invalid action
            }
            break;

        case ActionType::MineRock:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Rock) {
                actionPtr = std::make_unique<StoneAction>();
                actionReward = 10.0f;
                actionSuccess = true;
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::GatherBush:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Bush) {
                actionPtr = std::make_unique<BushAction>();
                actionReward = 10.0f;
                actionSuccess = true;
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::StoreItem:
            if (auto houseObj = dynamic_cast<House*>(tile.getObject())) {
                if (!inventory.empty()) {
                    for (const auto& [item, quantity] : inventory) {
                        houseObj->storeItem(item, quantity);
                    }
                    actionReward = 5.0f;
                    actionSuccess = true;
                } else {
                    actionReward = -5.0f; // Penalty for attempting to store nothing
                }
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::RegenerateEnergy:
            if (auto houseObj = dynamic_cast<House*>(tile.getObject())) {
                houseObj->regenerateEnergy(*this);
                actionReward = 5.0f;
                actionSuccess = true;
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::BuyItem:
            if (auto marketObj = dynamic_cast<Market*>(tile.getObject())) {
                if (getMoney() >= 10.0f) { // Example check
                    marketObj->buyItem(*this, "wood", 5);
                    actionReward = 5.0f;
                    actionSuccess = true;
                } else {
                    actionReward = -5.0f; // Not enough money penalty
                }
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::SellItem:
            if (auto marketObj = dynamic_cast<Market*>(tile.getObject())) {
                if (!inventory.empty()) {
                    for (const auto& [item, quantity] : inventory) {
                        if (item.empty()) {
                            getDebugConsole().log("ERROR", getName() + " has an EMPTY inventory item name.");
                            continue; // Skip invalid items
                        }
                        marketObj->sellItem(*this, item, quantity);
                    }
                    actionReward = 5.0f;
                    actionSuccess = true;
                } else {
                    getDebugConsole().log("ERROR", getName() + " tried to sell but inventory is EMPTY.");
                    actionReward = -5.0f;
                }
            } else {
                actionReward = -5.0f;
            }
            break;


        case ActionType::Rest:
            if (getEnergy() < getMaxEnergy()) {
                setEnergy(getMaxEnergy());
                actionReward = 5.0f;
                actionSuccess = true;
            } else {
                actionReward = -2.0f; // Penalty for unnecessary resting
            }
            break;

        case ActionType::None:
        default:
            actionReward = -10.0f; // Large penalty for doing nothing
            break;
    }

    if (actionPtr) {
        actionPtr->perform(*this, tile, tileMap);
        receiveFeedback(actionReward, tileMap); 
        getDebugConsole().log("Action", getName() + " performed: " + actionPtr->getActionName());
    } else {
        getDebugConsole().log("Action", getName() + " attempted action but it failed.");
    }
}





// void NPCEntity::performAction(ActionType action, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, Market& market, House& house) {
//     switch (action) {
//         case ActionType::ChopTree:
//             if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Tree) {
//                 TreeAction treeAction;
//                 treeAction.perform(*this, tile, tileMap);
//             }
//             break;

//         case ActionType::MineRock:
//             if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Rock) {
//                 StoneAction stoneAction;
//                 stoneAction.perform(*this, tile, tileMap);
//             }
//             break;

//         case ActionType::GatherBush:
//             if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Bush) {
//                 BushAction bushAction;
//                 bushAction.perform(*this, tile, tileMap);
//             }
//             break;

//         case ActionType::StoreItem:
//             for (const auto& item : inventory) {
//                 house.storeItem(item.first, item.second);
//             }
//             break;

//         case ActionType::UpgradeHouse:
//             house.upgrade(money, *this);
//             break;

//         case ActionType::RegenerateEnergy:
//             house.regenerateEnergy(*this);
//             break;

//         case ActionType::TakeOutItems:
//             for (const auto& item : house.getStorage()) {
//                 house.takeFromStorage(item.first, item.second, *this);
//             }
//             break;

//         case ActionType::BuyItem:
//             market.buyItem(*this, "food", 1); // Example: Buying food
//             break;

//         case ActionType::SellItem:
//             for (const auto& item : inventory) {
//                 market.sellItem(*this, item.first, item.second);
//             }
//             break;

//         case ActionType::Move:
//             position.x += speed * 0.1f; // Simple movement placeholder
//             break;

//         case ActionType::Rest:
//             house.regenerateEnergy(*this); // Rest regenerates energy
//             break;

//         case ActionType::None:
//         default:
//             break; // No action or idle state
//     }
// }


void NPCEntity::setTarget(Tile* newTarget) {
    target = newTarget;
}


bool NPCEntity::isAtTarget() const {
    if (target == nullptr) {
        return false;
    }
    return position == target->getPosition();
}

float& NPCEntity::getMoney() {
    return money; // Allow modification
}

const float& NPCEntity::getMoney() const {
    return money; // Read-only access
}


// Handle NPC Death
bool NPCEntity::isDead() const {
    return energy == 0.0f || health == 0.0f;
}

void NPCEntity::receiveFeedback(float reward, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (useQLearning) {
        // Extract correct Q-learning state
        State previousState = currentQLearningState;  
        State nextState = agent.extractState(tileMap, getPosition(), getEnergy(), getInventorySize(), getMaxInventorySize());
        
        // Update Q-learning table
        agent.updateQValue(previousState, lastAction, reward, nextState);

        // Update the current state
        currentQLearningState = nextState;
    }
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
    if (currentActionCooldown > 0) {
        currentActionCooldown -= deltaTime;
        currentActionCooldown = std::max(0.0f, currentActionCooldown);
    }

    if (energy > 0) {
        energy = std::max(0.0f, energy - deltaTime * 0.5f); // Energy decay
    }

    if (isDead()) {
        handleDeath();
    }

    getDebugConsole().log(name, "Updated NPC state for " + name);
}

Tile* NPCEntity::findNearestTile(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, ObjectType type) const {
    Tile* nearestTile = nullptr;
    float shortestDistance = std::numeric_limits<float>::max();

    for (int y = 0; y < tileMap.size(); ++y) {
        for (int x = 0; x < tileMap[y].size(); ++x) {
            if (tileMap[y][x]->hasObject() && tileMap[y][x]->getObject()->getType() == type) {
                float distance = std::hypot(
                    tileMap[y][x]->getPosition().x - getPosition().x,
                    tileMap[y][x]->getPosition().y - getPosition().y
                );
                if (distance < shortestDistance) {
                    shortestDistance = distance;
                    nearestTile = tileMap[y][x].get();
                }
            }
        }
    }

    return nearestTile;
}




Tile* NPCEntity::getTarget() const {
    return target;
}

// AI Decision-Making
ActionType NPCEntity::decideNextAction(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, const House& house) {
    // Step 0: Use Q-Learning if enabled
    if (useQLearning) {
        currentQLearningState = agent.extractState(tileMap, getPosition(), getEnergy(), getInventorySize(), getMaxInventorySize());
        lastAction = agent.decideAction(currentQLearningState);
        getDebugConsole().log("Decision", name + " (Q-Learning) decided: " + std::to_string(static_cast<int>(lastAction)));
        return lastAction;
    }

    // Step 1: Check if NPC has **Critical Needs** 
    if (getEnergy() < 15.0f) {
        getDebugConsole().log("Decision", name + " has critically low energy! Going to rest.");
        return ActionType::RegenerateEnergy;
    }

    if (getHunger() < 20.0f) {
        getDebugConsole().log("Decision", name + " is starving! Gathering food.");
        return ActionType::GatherBush;
    }

    if (getInventorySize() >= getMaxInventorySize()) {
        getDebugConsole().log("Decision", name + " inventory full, storing items.");
        return ActionType::StoreItem;
    }

    // Step 2: Check for the **Nearest Gatherable Resource**
    auto nearbyObjects = scanNearbyTiles(tileMap);
    std::shuffle(nearbyObjects.begin(), nearbyObjects.end(), std::mt19937{std::random_device{}()});

    for (ObjectType obj : nearbyObjects) {
        if (obj == ObjectType::Tree) {
            getDebugConsole().log("Decision", name + " chopping tree.");
            return ActionType::ChopTree;
        }
        if (obj == ObjectType::Rock) {
            getDebugConsole().log("Decision", name + " mining rock.");
            return ActionType::MineRock;
        }
        if (obj == ObjectType::Bush) {
            getDebugConsole().log("Decision", name + " gathering food.");
            return ActionType::GatherBush;
        }
    }

    // Step 3: **Check for Market - Trade Items if Possible**
    Tile* nearestMarket = findNearestTile(tileMap, ObjectType::Market);
    if (nearestMarket) {
        if (!inventory.empty()) {
            getDebugConsole().log("Decision", name + " selling items.");
            return ActionType::SellItem;
        }
        if (getMoney() >= 15.0f) {
            getDebugConsole().log("Decision", name + " buying resources.");
            return ActionType::BuyItem;
        }
    }

    // Step 4: **Check for House - Retrieve Stored Items**
    Tile* nearestHouse = findNearestTile(tileMap, ObjectType::House);
    if (nearestHouse) {
        if (getInventorySize() < getMaxInventorySize()) {
            getDebugConsole().log("Decision", name + " retrieving stored items.");
            return ActionType::TakeOutItems;
        }

        // Step 5: **House Upgrade Logic**
        if (house.isUpgradeAvailable(getMoney())) {
            getDebugConsole().log("Decision", name + " upgrading house.");
            return ActionType::UpgradeHouse;
        }
    }

    // Step 6: **Rest if energy is below 40**
    if (getEnergy() < 40.0f) {
        getDebugConsole().log("Decision", name + " slightly low energy, resting.");
        return ActionType::Rest;
    }

    // Step 7: **Final Fallback: Exploration**
    getDebugConsole().log("Decision", name + " no immediate action available, exploring.");
    return ActionType::Move;
}







// Extract State for Q-Learning
State NPCEntity::extractState(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) const {
    State state;
    state.posX = static_cast<int>(getPosition().x / GameConfig::tileSize);
    state.posY = static_cast<int>(getPosition().y / GameConfig::tileSize);
    state.nearbyTrees = countNearbyObjects(tileMap, ObjectType::Tree);
    state.nearbyRocks = countNearbyObjects(tileMap, ObjectType::Rock);
    state.nearbyBushes = countNearbyObjects(tileMap, ObjectType::Bush);
    state.energyLevel = agent.quantize(getEnergy(), 0, 100, 3);
    state.inventoryLevel = agent.quantize(getInventorySize(), 0, getMaxInventorySize(), 3);
    return state;
}

// Scan Nearby Tiles
std::vector<ObjectType> NPCEntity::scanNearbyTiles(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) const {
    std::vector<ObjectType> nearbyObjects;

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

// Count Nearby Objects
int NPCEntity::countNearbyObjects(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, ObjectType type) const {
    int count = 0;

    int npcX = static_cast<int>(getPosition().x / GameConfig::tileSize);
    int npcY = static_cast<int>(getPosition().y / GameConfig::tileSize);

    for (int y = npcY - 1; y <= npcY + 1; ++y) {
        for (int x = npcX - 1; x <= npcX + 1; ++x) {
            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                if (tileMap[y][x]->hasObject() && tileMap[y][x]->getObject()->getType() == type) {
                    count++;
                }
            }
        }
    }

    return count;
}
