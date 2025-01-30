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
      agent(0.1f, 0.9f, 0.1f),
      useQLearning(enableQLearning),
      name(npcName) {}

// Move Constructor
NPCEntity::NPCEntity(NPCEntity&& other) noexcept
    : Entity(std::move(other)),
      currentState(other.currentState),
      target(other.target),
      currentAction(other.currentAction),
      agent(std::move(other.agent)),
      useQLearning(other.useQLearning),
      inventory(std::move(other.inventory)),
      inventoryCapacity(other.inventoryCapacity),
      name(std::move(other.name)),
      baseSpeed(other.baseSpeed),
      currentSpeed(other.currentSpeed),
      deathPenalty(other.deathPenalty),
      currentReward(other.currentReward),
      currentPenalty(other.currentPenalty),
      currentActionCooldown(other.currentActionCooldown),
      house(other.house),
      lastAction(other.lastAction),
      currentQLearningState(std::move(other.currentQLearningState)) {}


// Move Assignment Operator
NPCEntity& NPCEntity::operator=(NPCEntity&& other) noexcept {
    if (this != &other) {
        Entity::operator=(std::move(other));
        currentState = other.currentState;
        target = other.target;
        currentAction = other.currentAction;
        agent = std::move(other.agent);
        useQLearning = other.useQLearning;
        inventory = std::move(other.inventory);
        inventoryCapacity = other.inventoryCapacity;
        name = std::move(other.name);
        baseSpeed = other.baseSpeed;
        currentSpeed = other.currentSpeed;
        deathPenalty = other.deathPenalty;
        currentReward = other.currentReward;
        currentPenalty = other.currentPenalty;
        currentActionCooldown = other.currentActionCooldown;
        house = other.house;
        lastAction = other.lastAction;
        currentQLearningState = std::move(other.currentQLearningState);
    }
    return *this;
}


// Getters
const std::string& NPCEntity::getName() const { return name; }
float NPCEntity::getMaxEnergy() const { return GameConfig::MAX_ENERGY; }
float NPCEntity::getBaseSpeed() const { return baseSpeed; }
float NPCEntity::getEnergyPercentage() const { return energy / GameConfig::MAX_ENERGY; }
const std::unordered_map<std::string, int>& NPCEntity::getInventory() const { return inventory; }
int NPCEntity::getMaxInventorySize() const { return inventoryCapacity; }
int NPCEntity::getInventorySize() const {
    return std::accumulate(inventory.begin(), inventory.end(), 0,
                           [](int total, const auto& pair) { return total + pair.second; });
}

void NPCEntity::updateQLearningState(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    currentQLearningState = extractState(tileMap);
}

int NPCEntity::getGatheredResources() const {
    return getInventoryItemCount("wood") + getInventoryItemCount("stone") + getInventoryItemCount("bush");
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

    std::string itemCopy = item;
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
    if (energy < GameConfig::MAX_ENERGY) {
        energy = std::min(GameConfig::MAX_ENERGY, energy + rate);
        getDebugConsole().log(name, name + "'s energy regenerated to " + std::to_string(energy));
    }
}

void NPCEntity::restoreHealth(float amount) {
    health = std::min(health + amount, GameConfig::MAX_HEALTH);
    getDebugConsole().log("HEALTH", name + " restored " + std::to_string(amount) + " health.");
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
                reduceHealth(0.8f);  // ✅ Health reduction
                consumeEnergy(1.5f);
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::MineRock:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Rock) {
                actionPtr = std::make_unique<StoneAction>();
                actionReward = 10.0f;
                actionSuccess = true;
                reduceHealth(1.2f);  // ✅ Health reduction
                consumeEnergy(1.5f);
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::GatherBush:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Bush) {
                actionPtr = std::make_unique<BushAction>();
                actionReward = 10.0f;
                actionSuccess = true;
                reduceHealth(0.5f);  // ✅ Health reduction
                consumeEnergy(1.5f);
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::StoreItem:
            if (auto houseObj = dynamic_cast<House*>(tile.getObject())) {
                if (!inventory.empty()) {
                    for (const auto& [item, quantity] : inventory) {
                        if (item == "wood" || item == "stone" || item == "bush") {
                            if (quantity > 2) {
                                houseObj->storeItem(item, quantity - 2);
                            }
                        } else {
                            houseObj->storeItem(item, quantity);
                        }
                    }
                    actionReward = 5.0f;
                    actionSuccess = true;
                } else {
                    actionReward = -5.0f;
                }
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::UpgradeHouse:
            if (getMoney() >= house.getUpgradeCost()) {
                bool hasAllItems = true;

                for (const std::string& item : {"wood", "stone", "bush"}) {
                    int requiredAmount = house.getRequirementForItem(item);
                    int inventoryAmount = getInventoryItemCount(item);
                    int storageAmount = house.getStoredItemCount(item);

                    if (inventoryAmount + storageAmount < requiredAmount) {
                        hasAllItems = false;
                        getDebugConsole().log("HOUSE", getName() + " is missing " +
                                            std::to_string(requiredAmount - (inventoryAmount + storageAmount)) +
                                            " " + item + " for upgrade.");
                    } else {
                        if (inventoryAmount > 0) {
                            int toStore = std::min(inventoryAmount, requiredAmount - storageAmount);
                            removeFromInventory(item, toStore);
                            house.storeItem(item, toStore);
                            getDebugConsole().log("HOUSE", getName() + " stored " + std::to_string(toStore) + " " + item + " in house.");
                        }
                    }
                }

                if (hasAllItems) {
                    if (house.upgrade(money, *this)) {
                        actionReward = 20.0f;
                        actionSuccess = true;
                        restoreHealth(10.0f);  
                        getDebugConsole().log("HOUSE", getName() + " successfully upgraded the house!");
                    } else {
                        actionReward = -5.0f;
                        getDebugConsole().log("HOUSE", getName() + " upgrade failed.");
                    }
                } else {
                    actionReward = -10.0f;
                }
            } else {
                actionReward = -10.0f;
                getDebugConsole().log("HOUSE", getName() + " lacks money to upgrade!");
            }
            break;

        case ActionType::RegenerateEnergy:
            if (auto houseObj = dynamic_cast<House*>(tile.getObject())) {
                houseObj->regenerateEnergy(*this);
                actionReward = 5.0f;
                actionSuccess = true;
                restoreHealth(3.0f);
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::BuyItem:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Market) {
                auto* marketObj = dynamic_cast<Market*>(tile.getObject());
                if (!marketObj) {
                    getDebugConsole().log("ERROR", "Market object is NULL. Skipping trade action.");
                    return;
                }

                std::vector<std::string> itemsToBuy = {"wood", "stone", "bush"};
                std::shuffle(itemsToBuy.begin(), itemsToBuy.end(), std::mt19937(std::random_device{}()));

                bool boughtSomething = false;
                for (const auto& item : itemsToBuy) {
                    float itemPrice = marketObj->calculateBuyPrice(item);
                    if (getMoney() >= itemPrice) {
                        int maxAffordable = static_cast<int>(getMoney() / itemPrice);
                        int quantityToBuy = std::min(maxAffordable, 3);

                        if (marketObj->buyItem(*this, item, quantityToBuy)) {
                            actionReward = 5.0f * quantityToBuy;
                            boughtSomething = true;
                            reduceHealth(1.5f);  // ✅ Small health reduction
                            consumeEnergy(1.0f);
                            getDebugConsole().log("MARKET", getName() + " bought " + std::to_string(quantityToBuy) + " " + item);
                            break;
                        }
                    }
                }

                if (!boughtSomething) {
                    getDebugConsole().log("ERROR", getName() + " could not afford anything.");
                    actionReward = -5.0f;
                }

                actionSuccess = boughtSomething;
            } else {
                getDebugConsole().log("ERROR", "Market tile is NULL or invalid.");
                actionReward = -5.0f;
            }
            break;

        case ActionType::SellItem:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Market) {
                auto* marketObj = dynamic_cast<Market*>(tile.getObject());
                if (!marketObj) {
                    getDebugConsole().log("ERROR", "Market object is NULL. Skipping trade action.");
                    return;
                }

                std::vector<std::string> itemsToSell;
                for (const auto& [item, quantity] : inventory) {
                    if (quantity > 1) {
                        itemsToSell.push_back(item);
                    }
                }

                if (!itemsToSell.empty()) {
                    static std::mt19937 rng(std::random_device{}());
                    std::shuffle(itemsToSell.begin(), itemsToSell.end(), rng);

                    std::uniform_int_distribution<int> sellQuantityDist(1, 3);
                    std::string selectedItem = itemsToSell.front();
                    int sellQuantity = std::min(sellQuantityDist(rng), getInventoryItemCount(selectedItem));

                    if (marketObj->sellItem(*this, selectedItem, sellQuantity)) {
                        actionReward = 10.0f * sellQuantity;
                        restoreHealth(2.0f);  // ✅ Small health reduction
                        consumeEnergy(1.0f);
                        getDebugConsole().log("MARKET", getName() + " sold " + std::to_string(sellQuantity) + " " + selectedItem);
                        actionSuccess = true;
                    }
                } else {
                    actionReward = -5.0f;
                    getDebugConsole().log("ERROR", getName() + " has nothing to sell.");
                }
            } else {
                actionReward = -5.0f;
                getDebugConsole().log("ERROR", "Market tile is NULL or invalid.");
            }
            break;

        case ActionType::Rest:
            if (getEnergy() < getMaxEnergy()) {
                setEnergy(getMaxEnergy());
                actionReward = 5.0f;
                actionSuccess = true;
                restoreHealth(5.0f);
            } else {
                actionReward = -2.0f;
            }
            break;

        default:
            actionReward = -10.0f;
            break;
    }

    if (actionPtr) {
        actionPtr->perform(*this, tile, tileMap);
        receiveFeedback(actionReward, tileMap);
    }
    setState(NPCState::Idle);
}

void NPCEntity::setTarget(Tile* newTarget) {
    if (newTarget == nullptr || !newTarget->hasObject()) {
        getDebugConsole().log("ERROR", getName() + " tried to target a NULL or empty tile.");
        target = nullptr;
        return;
    }

    if (newTarget->getObject()->getType() == ObjectType::Market) {
        auto* marketObj = dynamic_cast<Market*>(newTarget->getObject());
        if (!marketObj) {
            getDebugConsole().log("ERROR", "Market object is NULL. Preventing invalid assignment.");
            target = nullptr;
            return;
        }
    }
    
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


void NPCEntity::reduceHealth(float amount) {
    if (health > 5.0f) {  // Prevent instant deaths
        health -= amount;
        if (health <= 0.0f) {
            health = 0.0f;
            handleDeath();
        }
        getDebugConsole().log("HEALTH", getName() + " lost " + std::to_string(amount) + " health. Current: " + std::to_string(health));
    } else {
        getDebugConsole().log("HEALTH", getName() + " is too weak to take further damage.");
    }
}


bool NPCEntity::isDead() const {
    return health <= 0.0f;
}

// Handle NPC Death
void NPCEntity::handleDeath() {
    getDebugConsole().log(name, name + " has died.");
    addPenalty(deathPenalty);
}

void NPCEntity::receiveFeedback(float reward, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (useQLearning) {
        if (lastAction == ActionType::None) {
            lastAction = ActionType::Rest; // Default fallback action.
        }
        
        State previousState = currentQLearningState;  
        State nextState = agent.extractState(tileMap, getPosition(), getEnergy(), getInventorySize(), getMaxInventorySize());

        agent.updateQValue(previousState, lastAction, reward, nextState);
        currentQLearningState = nextState;
    }
}

// Inventory Capacity Upgrades
void NPCEntity::upgradeInventoryCapacity(int extraSlots) {
    inventoryCapacity += extraSlots;
    getDebugConsole().log(name, name + "'s inventory capacity upgraded to " + std::to_string(inventoryCapacity));
}

void NPCEntity::setHealth(float newHealth) {
    health = std::clamp(newHealth, 0.0f, GameConfig::MAX_HEALTH);
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
    int fallbackX = -1, fallbackY = -1;

    for (int y = 0; y < tileMap.size(); ++y) {
        for (int x = 0; x < tileMap[y].size(); ++x) {
            if (tileMap[y][x]->hasObject() && tileMap[y][x]->getObject()->getType() == type) {
                float distance = std::hypot(tileMap[y][x]->getPosition().x - getPosition().x,
                                            tileMap[y][x]->getPosition().y - getPosition().y);
                if (distance < shortestDistance) {
                    shortestDistance = distance;
                    nearestTile = tileMap[y][x].get();
                }
                fallbackX = x;
                fallbackY = y;
            }
        }
    }

    if (!nearestTile && fallbackX != -1) {
        getDebugConsole().log("ERROR", getName() + " could not find target. Using fallback tile.");
        return tileMap[fallbackY][fallbackX].get();
    }

    return nearestTile;
}

Tile* NPCEntity::getTarget() const {
    return target;
}

// AI Decision-Making
ActionType NPCEntity::decideNextAction(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, 
                                       const House& house, Market& market) {
    ActionType action = ActionType::None;  

    if (useQLearning) {
        currentQLearningState = agent.extractState(tileMap, getPosition(), getEnergy(), getInventorySize(), getMaxInventorySize());
        action = agent.decideAction(currentQLearningState);
    } else {
        if (getEnergy() < 10.0f) {
            action = ActionType::RegenerateEnergy;
        } 
        else if (getInventorySize() >= getMaxInventorySize()) {
            if (house.isStorageFull()) {
                // If house storage is also full, randomly sell items
                action = ActionType::SellItem;
            } else {
                action = ActionType::StoreItem;
            }
        }
        else if (getInventoryItemCount("wood") >= house.getWoodRequirement() &&
                getInventoryItemCount("stone") >= house.getStoneRequirement() &&
                getInventoryItemCount("bush") >= house.getBushRequirement()) {
            action = ActionType::StoreItem; // Store before upgrading
        } 
        else if (house.isUpgradeAvailable(getMoney())) {  
            action = ActionType::UpgradeHouse; 
        }
        else {
            std::string bestItemToSell = market.suggestBestResourceToSell();
            if (!bestItemToSell.empty() && getInventoryItemCount(bestItemToSell) > 2) {
                action = ActionType::SellItem;
            } 
            else {
                std::string bestItemToBuy = market.suggestBestResourceToBuy();
                if (!bestItemToBuy.empty() && getMoney() >= market.calculateBuyPrice(bestItemToBuy)) {
                    action = ActionType::BuyItem;
                } 
                else {
                    auto nearbyObjects = scanNearbyTiles(tileMap);
                    for (ObjectType obj : nearbyObjects) {
                        if (obj == ObjectType::Tree) action = ActionType::ChopTree;
                        if (obj == ObjectType::Rock) action = ActionType::MineRock;
                        if (obj == ObjectType::Bush) action = ActionType::GatherBush;
                    }
                }
            }
        }
    }

    if (action == ActionType::None) {
        static std::mt19937 rng(std::random_device{}()); 
        std::uniform_int_distribution<int> actionDist(1, static_cast<int>(ActionType::SellItem));
        action = static_cast<ActionType>(actionDist(rng));  // Randomize actions
        getDebugConsole().log("DEBUG", getName() + " was stuck, randomizing action to: " + std::to_string(static_cast<int>(action)));
    }



    return action;
}




void NPCEntity::setHouse(House* assignedHouse) {
    house = assignedHouse;
}

House* NPCEntity::getHouse() {
    if (!house) {
        getDebugConsole().log("ERROR", name + " has no house assigned!");
    }
    return house;
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
