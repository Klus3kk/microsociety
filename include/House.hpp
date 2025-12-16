#ifndef HOUSE_HPP
#define HOUSE_HPP

#include "Object.hpp"
#include "Entity.hpp"  
#include <unordered_map>
#include <string>
#include "TextureManager.hpp"

class NPCEntity;

class House : public Object {
private:
    int level;
    int maxStorageCapacity;
    float energyRegenRate;
    int healthBonus;
    int strengthBonus;
    int speedBonus;
    std::unordered_map<std::string, int> storage; // storage for items

    void logUpgradeDetails() const; // upgrade debug logs

public:
    explicit House(const sf::Texture& tex, int initialLevel = 1);

    // getters
    const std::unordered_map<std::string, int>& getStorage() const;
    int getWoodRequirement() const;
    int getStoneRequirement() const;
    int getBushRequirement() const;
    int getRequirementForItem(const std::string& item) const;
    int getLevel() const { return level; }
    int getMaxStorageCapacity() const { return maxStorageCapacity; }
    float getEnergyRegenRate() const { return energyRegenRate; }
    float getUpgradeCost() const; 
    
    void regenerateEnergy(Entity& entity);  // Energy regeneration for any entity
    bool storeItem(const std::string& item, int quantity); // Store resources
    bool takeFromStorage(const std::string& item, int quantity, Entity& entity); // Take resources
    bool upgrade(float& entityMoney, Entity& entity); // Upgrade house level

    void displayStorage() const; // Display storage details
    void displayStats() const;   // Display house stats
    void resetDailyLimits();     // Reset any daily limits (if applicable)
    
    // AI Integration
    bool isStorageFull() const;  // Check if storage is full
    bool isUpgradeAvailable(float entityMoney) const; // Check if entity can afford an upgrade
    int getStoredItemCount(const std::string& item) const; // Get quantity of a specific resource

    // Rendering
    void draw(sf::RenderWindow& window) override; // Render house
    ObjectType getType() const override;         // Return object type
};

#endif
