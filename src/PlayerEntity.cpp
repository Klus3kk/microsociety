#include "PlayerEntity.hpp"
#include "Actions.hpp"
#include "Market.hpp"
#include "debug.hpp"
#include "House.hpp"
#include <algorithm>
#include <cmath>

// Constructor
PlayerEntity::PlayerEntity(const std::string& playerName, float initHealth, float initHunger, 
                           float initEnergy, float initSpeed, float initStrength, float initMoney)
    : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney),
      name(playerName) {
    
    // Initialize camera
    playerCamera.setSize(800.f, 600.f);
    playerCamera.setCenter(position);
    
    getDebugConsole().log("Player", "Created player entity: " + name);
}

// Movement
void PlayerEntity::handleInput(float deltaTime) {
    float moveX = 0.0f;
    float moveY = 0.0f;
    
    // Check keyboard input for movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveY -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveY += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveX -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveX += 1.0f;
    
    // Normalize diagonal movement
    if (moveX != 0.0f && moveY != 0.0f) {
        float length = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= length;
        moveY /= length;
    }
    
    // Apply movement
    if (moveX != 0.0f || moveY != 0.0f) {
        move(moveX, moveY, deltaTime);
        consumeEnergy(0.1f * deltaTime); // Consume energy while moving
    }
}

void PlayerEntity::update(float deltaTime) {
    if (energy > 0) {
        energy = std::max(0.0f, energy - deltaTime * 0.1f); // Energy decays slowly over time
    }
    
    // Check for death conditions
    if (health <= 0.0f || energy <= 0.0f) {
        health = 0.0f;
        energy = 0.0f;
        setDead(true);
        getDebugConsole().log("Player", name + " has died!");
    }
}

// Inventory Management
bool PlayerEntity::addToInventory(const std::string& item, int quantity) {
    if (getInventorySize() + quantity > inventoryCapacity) {
        getDebugConsole().log("Inventory", name + "'s inventory full! Cannot add " + item + ".");
        return false;
    }
    inventory[item] += quantity;
    getDebugConsole().log("Inventory", name + " added " + std::to_string(quantity) + " " + item + "(s) to inventory.");
    return true;
}

bool PlayerEntity::removeFromInventory(const std::string& item, int quantity) {
    auto it = inventory.find(item);
    if (it == inventory.end() || it->second < quantity) {
        getDebugConsole().log("ERROR", name + " does not have enough " + item + " to remove.");
        return false;
    }
    
    it->second -= quantity;
    if (it->second <= 0) inventory.erase(it); // Remove if quantity reaches zero
    getDebugConsole().log("Inventory", name + " removed " + std::to_string(quantity) + " " + item);
    return true;
}

int PlayerEntity::getInventoryItemCount(const std::string& item) const {
    auto it = inventory.find(item);
    return (it != inventory.end()) ? it->second : 0;
}

int PlayerEntity::getInventorySize() const {
    return std::accumulate(inventory.begin(), inventory.end(), 0,
                           [](int total, const auto& pair) { return total + pair.second; });
}

int PlayerEntity::getMaxInventorySize() const {
    return inventoryCapacity;
}

const std::unordered_map<std::string, int>& PlayerEntity::getInventory() const {
    return inventory;
}

// Interaction with world
void PlayerEntity::performAction(ActionType action, Tile& tile, 
                               const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                               Market& market, House& house) {
    std::unique_ptr<Action> actionPtr = nullptr;
    
    switch (action) {
        case ActionType::ChopTree:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Tree) {
                actionPtr = std::make_unique<TreeAction>();
                reduceHealth(0.5f);
                consumeEnergy(1.0f);
            }
            break;
            
        case ActionType::MineRock:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Rock) {
                actionPtr = std::make_unique<StoneAction>();
                reduceHealth(0.8f);
                consumeEnergy(1.2f);
            }
            break;
            
        case ActionType::GatherBush:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Bush) {
                actionPtr = std::make_unique<BushAction>();
                reduceHealth(0.3f);
                consumeEnergy(0.8f);
            }
            break;
            
        case ActionType::BuyItem:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Market) {
                auto* marketObj = dynamic_cast<Market*>(tile.getObject());
                if (!marketObj) {
                    getDebugConsole().log("ERROR", "Market object is NULL.");
                    return;
                }
                
                // Sample buy logic - could use UI to select item and quantity
                std::string itemToBuy = "wood"; // For demo purposes
                int quantityToBuy = 1;
                
                if (marketObj->buyItem(*this, itemToBuy, quantityToBuy)) {
                    consumeEnergy(0.5f);
                    getDebugConsole().log("MARKET", name + " bought " + std::to_string(quantityToBuy) + " " + itemToBuy);
                }
            }
            break;
            
        case ActionType::SellItem:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Market) {
                auto* marketObj = dynamic_cast<Market*>(tile.getObject());
                if (!marketObj) {
                    getDebugConsole().log("ERROR", "Market object is NULL.");
                    return;
                }
                
                // Get first item with quantity > 0
                std::string itemToSell;
                for (const auto& [item, quantity] : inventory) {
                    if (quantity > 0) {
                        itemToSell = item;
                        break;
                    }
                }
                
                if (!itemToSell.empty()) {
                    int sellQuantity = 1;
                    if (marketObj->sellItem(*this, itemToSell, sellQuantity)) {
                        consumeEnergy(0.5f);
                        getDebugConsole().log("MARKET", name + " sold " + std::to_string(sellQuantity) + " " + itemToSell);
                    }
                } else {
                    getDebugConsole().log("MARKET", name + " has nothing to sell.");
                }
            }
            break;
            
        case ActionType::RegenerateEnergy:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::House) {
                auto* houseObj = dynamic_cast<House*>(tile.getObject());
                if (houseObj) {
                    houseObj->regenerateEnergy(*this);
                    getDebugConsole().log("HOUSE", name + " regenerated energy at house.");
                }
            }
            break;
            
        case ActionType::UpgradeHouse:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::House) {
                auto* houseObj = dynamic_cast<House*>(tile.getObject());
                if (houseObj) {
                    // FIXED: Use temporary variable for money reference
                    float playerMoney = getMoney();
                    if (houseObj->upgrade(playerMoney, *this)) {
                        setMoney(playerMoney); // Update money after upgrade
                        getDebugConsole().log("HOUSE", name + " upgraded house successfully!");
                    }
                }
            }
            break;
            
        case ActionType::StoreItem:
            if (tile.hasObject() && tile.getObject()->getType() == ObjectType::House) {
                auto* houseObj = dynamic_cast<House*>(tile.getObject());
                if (houseObj) {
                    // Store the first item in inventory
                    for (const auto& [item, quantity] : inventory) {
                        if (quantity > 0) {
                            int storeQty = quantity / 2; // Store half of the item quantity
                            if (storeQty > 0 && houseObj->storeItem(item, storeQty)) {
                                removeFromInventory(item, storeQty);
                                getDebugConsole().log("HOUSE", name + " stored " + std::to_string(storeQty) + " " + item);
                                break;
                            }
                        }
                    }
                }
            }
            break;
            
        default:
            getDebugConsole().log("ACTION", "Unsupported action requested by player.");
            break;
    }
    
    if (actionPtr) {
        actionPtr->perform(*this, tile, tileMap);
    }
}

void PlayerEntity::interactWithTile(Tile& tile, 
                                   const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                                   Market& market, House& house) {
    if (!tile.hasObject()) {
        getDebugConsole().log("INTERACT", "Nothing to interact with here.");
        return;
    }
    
    ObjectType objType = tile.getObject()->getType();
    ActionType action = ActionType::None;
    
    switch (objType) {
        case ObjectType::Tree:
            action = ActionType::ChopTree;
            break;
        case ObjectType::Rock:
            action = ActionType::MineRock;
            break;
        case ObjectType::Bush:
            action = ActionType::GatherBush;
            break;
        case ObjectType::House:
            // Could show a UI to select between different house actions
            action = ActionType::RegenerateEnergy;
            break;
        case ObjectType::Market:
            // Could show a UI to select between buy/sell
            action = ActionType::BuyItem;
            break;
        default:
            getDebugConsole().log("INTERACT", "Unknown object type.");
            return;
    }
    
    if (action != ActionType::None) {
        performAction(action, tile, tileMap, market, house);
    }
}

// Energy Management
void PlayerEntity::consumeEnergy(float amount) {
    energy = std::max(0.0f, energy - amount);
    if (energy <= 0.0f) {
        getDebugConsole().log("ENERGY", name + " has run out of energy!");
    }
}

void PlayerEntity::regenerateEnergy(float rate) {
    float oldEnergy = energy;
    energy = std::min(GameConfig::MAX_ENERGY, energy + rate);
    
    if (energy > oldEnergy) {
        getDebugConsole().log("ENERGY", name + "'s energy regenerated to " + std::to_string(energy));
    }
}

// Health Management  
void PlayerEntity::restoreHealth(float amount) {
    health = std::min(health + amount, GameConfig::MAX_HEALTH);
    getDebugConsole().log("HEALTH", name + " restored " + std::to_string(amount) + " health.");
}

void PlayerEntity::reduceHealth(float amount) {
    if (health > 5.0f) {  // Prevent instant deaths
        health -= amount;
        if (health <= 0.0f) {
            health = 0.0f;
            getDebugConsole().log("HEALTH", name + " has died.");
        } else {
            getDebugConsole().log("HEALTH", name + " lost " + std::to_string(amount) + " health. Current: " + std::to_string(health));
        }
    } else {
        getDebugConsole().log("HEALTH", name + " is too weak to take further damage.");
    }
}

// Camera management
void PlayerEntity::updateCamera(sf::RenderWindow& window, float mapWidth, float mapHeight) {
    if (!cameraActive) return;
    
    // Set camera center to player position
    playerCamera.setCenter(position);
    
    // Ensure camera stays within map bounds
    float viewSizeX = playerCamera.getSize().x / 2;
    float viewSizeY = playerCamera.getSize().y / 2;
    
    // Map bounds (in pixels)
    float mapWidthPixels = mapWidth * GameConfig::tileSize;
    float mapHeightPixels = mapHeight * GameConfig::tileSize;
    
    // Adjust camera position to keep it within bounds
    sf::Vector2f cameraCenter = playerCamera.getCenter();
    
    // Clamp X position
    if (cameraCenter.x - viewSizeX < 0) {
        cameraCenter.x = viewSizeX;
    } else if (cameraCenter.x + viewSizeX > mapWidthPixels) {
        cameraCenter.x = mapWidthPixels - viewSizeX;
    }
    
    // Clamp Y position
    if (cameraCenter.y - viewSizeY < 0) {
        cameraCenter.y = viewSizeY;
    } else if (cameraCenter.y + viewSizeY > mapHeightPixels) {
        cameraCenter.y = mapHeightPixels - viewSizeY;
    }
    
    playerCamera.setCenter(cameraCenter);
    window.setView(playerCamera);
}

void PlayerEntity::activateCamera(bool active) {
    cameraActive = active;
}

sf::View& PlayerEntity::getCamera() {
    return playerCamera;
}

// Getters
const std::string& PlayerEntity::getName() const {
    return name;
}

float PlayerEntity::getMaxEnergy() const {
    return GameConfig::MAX_ENERGY;
}

float PlayerEntity::getEnergyPercentage() const {
    return energy / GameConfig::MAX_ENERGY;
}

Tile* PlayerEntity::getTargetTileAt(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, 
                                   float x, float y) const {
    // Convert world coordinates to tile indices
    int tileX = static_cast<int>(x / GameConfig::tileSize);
    int tileY = static_cast<int>(y / GameConfig::tileSize);
    
    // Check bounds
    if (tileX >= 0 && tileX < tileMap[0].size() && tileY >= 0 && tileY < tileMap.size()) {
        return tileMap[tileY][tileX].get();
    }
    
    return nullptr;
}