#ifndef PLAYER_ENTITY_HPP
#define PLAYER_ENTITY_HPP

#include "Entity.hpp"
#include "Tile.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include "ActionType.hpp"

// Forward declarations
class Market;
class House;

class PlayerEntity : public Entity {
private:
    std::string name;
    std::unordered_map<std::string, int> inventory;
    int inventoryCapacity = 15; // Player has slightly larger inventory than NPCs
    
    Tile* targetTile = nullptr;
    ActionType currentAction = ActionType::None;
    
    float baseSpeed = 200.0f; // Player moves slightly faster than NPCs
    
    // Camera
    sf::View playerCamera;
    bool cameraActive = true;

public:
    // Constructor
    PlayerEntity(const std::string& playerName, float initHealth, float initHunger, 
                float initEnergy, float initSpeed, float initStrength, float initMoney);
                
    // Movement
    void handleInput(float deltaTime);
    void update(float deltaTime);
    
    // Inventory Management
    bool addToInventory(const std::string& item, int quantity);
    bool removeFromInventory(const std::string& item, int quantity);
    int getInventoryItemCount(const std::string& item) const;
    int getInventorySize() const;
    int getMaxInventorySize() const;
    const std::unordered_map<std::string, int>& getInventory() const;
    
    // Interaction with world
    void performAction(ActionType action, Tile& tile, 
                       const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                       Market& market, House& house);
    
    void interactWithTile(Tile& tile, 
                         const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap,
                         Market& market, House& house);
    
    // Energy Management
    void consumeEnergy(float amount);
    void regenerateEnergy(float rate);
    
    // Health Management
    void restoreHealth(float amount);
    void reduceHealth(float amount);
    
    // Camera 
    void updateCamera(sf::RenderWindow& window, float mapWidth, float mapHeight);
    void activateCamera(bool active);
    sf::View& getCamera();
    
    // Getters
    const std::string& getName() const;
    float getMaxEnergy() const;
    float getEnergyPercentage() const;
    
    // Detection helper
    Tile* getTargetTileAt(const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap, 
                         float x, float y) const;
};

#endif // PLAYER_ENTITY_HPP