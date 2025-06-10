#include "NPCEntity.hpp"
#include "House.hpp"
#include "Market.hpp" 
#include "Actions.hpp"
#include "DataCollector.hpp"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
// Constructor
NPCEntity::NPCEntity(const std::string& npcName, float initHealth, float initHunger, float initEnergy,
                     float initSpeed, float initStrength, float initMoney, bool enableQLearning)
    : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney),
      agent(0.1f, 0.9f, 0.3f),  // FIXED: Increased epsilon for more exploration
      useQLearning(enableQLearning),
      name(npcName) {
    
    // FIXED: Ensure NPCs start in a valid state
    currentState = NPCState::Idle;
    target = nullptr;
    currentAction = ActionType::None;
    currentActionCooldown = 0.0f;
    
    getDebugConsole().log("NPC", "Created " + name + " with Q-Learning: " + 
                        (enableQLearning ? "ENABLED" : "DISABLED"));
}

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
        getDebugConsole().log("ERROR", name + " removeFromInventory() received an EMPTY item name.");
        return false;
    }

    auto it = inventory.find(item);
    if (it == inventory.end()) {
        getDebugConsole().log("ERROR", name + " attempted to remove an item that DOES NOT EXIST: " + item);
        return false;
    }

    if (it->second < quantity) {
        getDebugConsole().log("ERROR", name + " tried to remove more items than they HAVE: " + item);
        return false;
    }

    // FIXED: Store item name BEFORE modifying iterator
    std::string itemName = it->first;
    it->second -= quantity;
    if (it->second <= 0) {
        inventory.erase(it); // Remove if quantity reaches zero
    }

    getDebugConsole().log("Inventory", name + " removed " + std::to_string(quantity) + " " + itemName);
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

    // FIXED: Add action cooldown to prevent spam
    if (currentActionCooldown > 0) {
        getDebugConsole().log("Action", getName() + " is on cooldown, skipping action");
        return;
    }

    switch (action) {
        case ActionType::ChopTree:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Tree) {
                actionPtr = std::make_unique<TreeAction>();
                actionReward = 10.0f;
                actionSuccess = true;
                reduceHealth(0.5f);  // FIXED: Reduced damage
                consumeEnergy(3.0f); // FIXED: Reasonable energy cost
                currentActionCooldown = 1.0f; // FIXED: Add cooldown
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::MineRock:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Rock) {
                actionPtr = std::make_unique<StoneAction>();
                actionReward = 10.0f;
                actionSuccess = true;
                reduceHealth(0.8f);
                consumeEnergy(4.0f);
                currentActionCooldown = 1.2f;
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::GatherBush:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Bush) {
                actionPtr = std::make_unique<BushAction>();
                actionReward = 8.0f;
                actionSuccess = true;
                reduceHealth(0.3f);
                consumeEnergy(2.0f);
                currentActionCooldown = 0.8f;
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::RegenerateEnergy:
            // FIXED: Only allow energy regeneration at houses, with limits
            if (auto houseObj = dynamic_cast<House*>(tile.getObject())) {
                if (getEnergy() < getMaxEnergy() * 0.8f) { // Only regenerate if below 80%
                    float energyBefore = getEnergy();
                    houseObj->regenerateEnergy(*this);
                    float energyGained = getEnergy() - energyBefore;
                    
                    if (energyGained > 0) {
                        actionReward = 5.0f;
                        actionSuccess = true;
                        restoreHealth(1.0f); // FIXED: Reduced healing
                        currentActionCooldown = 2.0f; // FIXED: Longer cooldown to prevent abuse
                        getDebugConsole().log("Energy", getName() + " regenerated " + 
                                            std::to_string(energyGained) + " energy at house");
                    } else {
                        actionReward = -2.0f; // Penalty for unnecessary regeneration
                    }
                } else {
                    actionReward = -5.0f; // Penalty for trying to regenerate when not needed
                    getDebugConsole().log("Energy", getName() + " tried to regenerate but energy is already high");
                }
            } else {
                actionReward = -10.0f; // Big penalty for trying to regenerate without house
                getDebugConsole().log("Error", getName() + " tried to regenerate energy without a house");
            }
            break;

        case ActionType::BuyItem:
            // FIXED: Proper market buying with tracking
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Market) {
                auto* marketObj = dynamic_cast<Market*>(tile.getObject());
                if (marketObj) {
                    std::vector<std::string> itemsToBuy = {"wood", "stone", "bush"};
                    
                    bool boughtSomething = false;
                    for (const auto& item : itemsToBuy) {
                        float itemPrice = marketObj->calculateBuyPrice(item);
                        if (getMoney() >= itemPrice && getInventorySize() < getMaxInventorySize()) {
                            int quantityToBuy = 1; // FIXED: Buy one at a time
                            
                            if (marketObj->buyItem(*this, item, quantityToBuy)) {
                                actionReward = 8.0f;
                                boughtSomething = true;
                                consumeEnergy(1.0f);
                                currentActionCooldown = 1.5f;
                                getDebugConsole().log("MARKET", getName() + " bought " + 
                                                    std::to_string(quantityToBuy) + " " + item + 
                                                    " for $" + std::to_string(itemPrice));
                                break;
                            }
                        }
                    }
                    
                    if (!boughtSomething) {
                        actionReward = -3.0f;
                        getDebugConsole().log("MARKET", getName() + " couldn't buy anything (money: " + 
                                            std::to_string(getMoney()) + ", inventory: " + 
                                            std::to_string(getInventorySize()) + "/" + 
                                            std::to_string(getMaxInventorySize()) + ")");
                    }
                    actionSuccess = boughtSomething;
                } else {
                    actionReward = -5.0f;
                }
            } else {
                actionReward = -5.0f;
            }
            break;

            case ActionType::SellItem:
            // FIXED: Proper market selling with tracking and null checks
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Market) {
                auto* marketObj = dynamic_cast<Market*>(tile.getObject());
                if (marketObj) {
                    // Find items to sell (keep some for upgrades) - safer iteration
                    std::vector<std::pair<std::string, int>> itemsToSell;
                    for (const auto& inventoryPair : inventory) {
                        const std::string& itemName = inventoryPair.first;
                        int quantity = inventoryPair.second;
                        if (quantity > 2) { // Keep 2 of each item
                            itemsToSell.push_back({itemName, std::min(quantity - 2, 3)}); // Sell max 3
                        }
                    }
                    
                    bool soldSomething = false;
                    if (!itemsToSell.empty()) {
                        const auto& sellPair = itemsToSell[0];
                        const std::string& selectedItem = sellPair.first;
                        int sellQuantity = sellPair.second;
                        
                        // Validate we still have the item before selling
                        if (getInventoryItemCount(selectedItem) >= sellQuantity) {
                            float expectedRevenue = marketObj->calculateSellPrice(selectedItem) * sellQuantity;
                            
                            if (marketObj->sellItem(*this, selectedItem, sellQuantity)) {
                                actionReward = 12.0f;
                                soldSomething = true;
                                restoreHealth(1.0f);
                                consumeEnergy(1.0f);
                                currentActionCooldown = 1.5f;
                                getDebugConsole().log("MARKET", getName() + " sold " + 
                                                    std::to_string(sellQuantity) + " " + selectedItem + 
                                                    " for $" + std::to_string(expectedRevenue));
                            }
                        } else {
                            getDebugConsole().log("MARKET", getName() + " inventory changed, cannot sell " + selectedItem);
                        }
                    }
                    
                    if (!soldSomething) {
                        actionReward = -3.0f;
                        getDebugConsole().log("MARKET", getName() + " has nothing valuable to sell");
                    }
                    actionSuccess = soldSomething;
                } else {
                    actionReward = -5.0f;
                }
            } else {
                actionReward = -5.0f;
            }
            break;
            
        case ActionType::StoreItem:
            if (auto houseObj = dynamic_cast<House*>(tile.getObject())) {
                bool storedSomething = false;
                // FIXED: Safer inventory iteration to avoid iterator invalidation
                std::vector<std::pair<std::string, int>> inventorySnapshot;
                for (const auto& inventoryPair : inventory) {
                    inventorySnapshot.push_back({inventoryPair.first, inventoryPair.second});
                }
                
                for (const auto& itemPair : inventorySnapshot) {
                    const std::string& itemName = itemPair.first;
                    int quantity = itemPair.second;
                    if (quantity > 0) {
                        int storeAmount = std::min(quantity, 5); // Store up to 5 at a time
                        // Verify we still have this amount
                        if (getInventoryItemCount(itemName) >= storeAmount) {
                            if (houseObj->storeItem(itemName, storeAmount)) {
                                if (removeFromInventory(itemName, storeAmount)) {
                                    actionReward = 3.0f * storeAmount;
                                    storedSomething = true;
                                    currentActionCooldown = 1.0f;
                                    getDebugConsole().log("HOUSE", getName() + " stored " + 
                                                        std::to_string(storeAmount) + " " + itemName);
                                    break; // Store one type at a time
                                }
                            }
                        }
                    }
                }
                
                if (!storedSomething) {
                    actionReward = -3.0f;
                }
                actionSuccess = storedSomething;
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::UpgradeHouse:
            if (getMoney() >= house.getUpgradeCost()) {
                float npcMoney = getMoney();
                if (house.upgrade(npcMoney, *this)) {
                    setMoney(npcMoney);
                    actionReward = 25.0f;
                    actionSuccess = true;
                    restoreHealth(10.0f);
                    currentActionCooldown = 3.0f; // Long cooldown for upgrades
                    getDebugConsole().log("HOUSE", getName() + " successfully upgraded the house!");
                } else {
                    actionReward = -8.0f;
                }
            } else {
                actionReward = -5.0f;
            }
            break;

        case ActionType::Rest:
            if (getEnergy() < getMaxEnergy() * 0.5f) { // Only rest if energy is below 50%
                float energyBefore = getEnergy();
                setEnergy(std::min(getMaxEnergy(), getEnergy() + 20.0f)); // Partial rest
                actionReward = 3.0f;
                actionSuccess = true;
                restoreHealth(2.0f);
                currentActionCooldown = 2.0f;
                getDebugConsole().log("Rest", getName() + " rested and gained " + 
                                    std::to_string(getEnergy() - energyBefore) + " energy");
            } else {
                actionReward = -3.0f; // Penalty for unnecessary rest
            }
            break;

        default:
            actionReward = -10.0f;
            break;
    }

    // Execute the action if we have one
    if (actionPtr) {
        actionPtr->perform(*this, tile, tileMap);
    }
    
    // FIXED: Always provide feedback for learning
    receiveFeedback(actionReward, tileMap);
    
    // FIXED: Set state back to idle properly
    setState(NPCState::Idle);
    
    // Record the action for stuck detection
    lastAction = action;
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

        // NEW: Record experience for training data collection
        if (useTensorFlow) {
            // Determine if this is a terminal state
            bool isTerminal = (health <= 0.0f) || (energy <= 0.0f) || (getInventorySize() >= getMaxInventorySize());
            
            // Record the experience
            getDataCollector().recordExperience(
                previousState,      // Current state
                lastAction,         // Action taken
                reward,            // Reward received
                nextState,         // Next state
                isTerminal,        // Done flag
                getName()          // NPC name for tracking
            );
            
            getDebugConsole().log("DataCollection", getName() + " recorded experience: " + 
                                std::to_string(static_cast<int>(lastAction)) + " -> reward: " + std::to_string(reward));
        }

        // Continue with existing Q-learning update
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
    // FIXED: Update cooldowns properly
    if (currentActionCooldown > 0) {
        currentActionCooldown -= deltaTime;
        currentActionCooldown = std::max(0.0f, currentActionCooldown);
    }

    // FIXED: Gradual energy decay (not too fast)
    if (energy > 0) {
        float energyDecay = deltaTime * 2.0f; // Slower decay
        energy = std::max(0.0f, energy - energyDecay);
    }

    // FIXED: Health regeneration over time (slow)
    if (health > 0 && health < getMaxEnergy()) {
        float healthRegen = deltaTime * 0.5f;
        health = std::min(GameConfig::MAX_HEALTH, health + healthRegen);
    }

    // Check for death
    if (health <= 0.0f || energy <= 0.0f) {
        setDead(true);
        handleDeath();
    }
}

Tile* NPCEntity::findNearestTile(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, ObjectType type) const {
    Tile* nearestTile = nullptr;
    float shortestDistance = std::numeric_limits<float>::max();
    std::vector<Tile*> allValidTiles;

    // Collect all valid tiles
    for (int y = 0; y < tileMap.size(); ++y) {
        for (int x = 0; x < tileMap[y].size(); ++x) {
            if (tileMap[y][x]->hasObject() && tileMap[y][x]->getObject()->getType() == type) {
                float distance = std::hypot(tileMap[y][x]->getPosition().x - getPosition().x,
                                            tileMap[y][x]->getPosition().y - getPosition().y);
                
                allValidTiles.push_back(tileMap[y][x].get());
                
                if (distance < shortestDistance) {
                    shortestDistance = distance;
                    nearestTile = tileMap[y][x].get();
                }
            }
        }
    }

    // FIXED: If no nearest tile found but we have valid tiles, pick randomly
    if (!nearestTile && !allValidTiles.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, allValidTiles.size() - 1);
        nearestTile = allValidTiles[dist(gen)];
        getDebugConsole().log("Pathfinding", getName() + " using random valid tile as fallback");
    }

    return nearestTile;
}

Tile* NPCEntity::getTarget() const {
    return target;
}

// TensorFlow integration methods
void NPCEntity::enableTensorFlow(bool enable) {
    useTensorFlow = enable && tfModel && tfModel->isModelLoaded();
    if (enable && (!tfModel || !tfModel->isModelLoaded())) {
        getDebugConsole().log("TensorFlow", getName() + " could not enable TensorFlow (model not available)", LogLevel::Warning);
    } else if (enable) {
        getDebugConsole().log("TensorFlow", getName() + " is now using TensorFlow for decision making");
    }
}

void NPCEntity::setTensorFlowModel(std::shared_ptr<TensorFlowWrapper> model) {
    tfModel = model;
    // Re-validate TensorFlow status
    if (useTensorFlow && (!tfModel || !tfModel->isModelLoaded())) {
        useTensorFlow = false;
        getDebugConsole().log("TensorFlow", getName() + " disabled TensorFlow (model not available)", LogLevel::Warning);
    }
}

// AI Decision Making
ActionType NPCEntity::decideNextAction(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, 
                                    const House& house, Market& market) {
    ActionType action = ActionType::None;
    
    // FIXED: Add stuck detection
    static std::unordered_map<std::string, int> stuckCounter;
    static std::unordered_map<std::string, ActionType> lastActionMap;
    
    if (useTensorFlow && tfModel && tfModel->isModelLoaded()) {
        currentQLearningState = extractState(tileMap);
        action = tfModel->predictAction(currentQLearningState);
        getDebugConsole().log("TensorFlow", getName() + " used TF model to choose action: " + 
                            std::to_string(static_cast<int>(action)));
    }
    else if (useTensorFlow && !tfModel) {
        // DATA COLLECTION MODE: More structured exploration
        std::random_device rd;
        std::mt19937 gen(rd());
        
        // FIXED: More intelligent data collection
        if (getEnergy() < 20.0f) {
            action = ActionType::RegenerateEnergy;
        } else if (getInventorySize() >= getMaxInventorySize() - 1) {
            std::uniform_int_distribution<int> storeSellDist(0, 1);
            action = (storeSellDist(gen) == 0) ? ActionType::StoreItem : ActionType::SellItem;
        } else if (getMoney() > 50.0f && getInventorySize() < 3) {
            action = ActionType::BuyItem;
        } else {
            // Exploration actions
            std::uniform_int_distribution<int> actionDist(2, 4); // ChopTree, MineRock, GatherBush
            action = static_cast<ActionType>(actionDist(gen));
        }
        
        getDebugConsole().log("DataCollection", getName() + " using exploration action: " + 
                            std::to_string(static_cast<int>(action)));
    }
    else if (useQLearning) {
        currentQLearningState = agent.extractState(tileMap, getPosition(), getEnergy(), getInventorySize(), getMaxInventorySize());
        action = agent.decideAction(currentQLearningState);
        
        // FIXED: Anti-stuck mechanism for Q-learning
        if (lastActionMap[name] == action) {
            stuckCounter[name]++;
            if (stuckCounter[name] > 3) {
                // Force different action
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> actionDist(1, static_cast<int>(ActionType::Rest));
                action = static_cast<ActionType>(actionDist(gen));
                stuckCounter[name] = 0;
                getDebugConsole().log("Q-Learning", getName() + " was stuck, forced random action");
            }
        } else {
            stuckCounter[name] = 0;
        }
        lastActionMap[name] = action;
    }
    else {
        // FIXED: Better rule-based behavior with variety
        std::random_device rd;
        std::mt19937 gen(rd());
        
        if (getEnergy() < 15.0f) {
            action = ActionType::RegenerateEnergy;
        } 
        else if (getInventorySize() >= getMaxInventorySize()) {
            // FIXED: Prefer selling over storing sometimes
            std::uniform_int_distribution<int> choice(0, 2);
            if (choice(gen) == 0 || house.isStorageFull()) {
                action = ActionType::SellItem;
            } else {
                action = ActionType::StoreItem;
            }
        }
        else if (getMoney() > 100.0f && getInventorySize() < 3) {
            action = ActionType::BuyItem;
        }
        else if (house.isUpgradeAvailable(getMoney())) {  
            action = ActionType::UpgradeHouse; 
        }
        else {
            // FIXED: More variety in resource gathering
            std::uniform_int_distribution<int> resourceChoice(0, 2);
            switch (resourceChoice(gen)) {
                case 0: action = ActionType::ChopTree; break;
                case 1: action = ActionType::MineRock; break;
                case 2: action = ActionType::GatherBush; break;
            }
        }
    }

    // FIXED: Final fallback with better randomization
    if (action == ActionType::None) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> actionDist(1, static_cast<int>(ActionType::Rest));
        action = static_cast<ActionType>(actionDist(gen));
        getDebugConsole().log("DEBUG", getName() + " using fallback random action: " + 
                            std::to_string(static_cast<int>(action)));
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

void NPCEntity::enableQLearning(bool enable) {
    useQLearning = enable;
}