#ifndef HOUSE_HPP
#define HOUSE_HPP

#include "Object.hpp"
#include "NPCEntity.hpp"
#include <unordered_map>
#include <string>
#include "TextureManager.hpp"

class House : public Object {
private:
    int level;
    int maxStorageCapacity;
    float energyRegenRate;
    int healthBonus;
    int strengthBonus;
    int speedBonus;
    std::unordered_map<std::string, int> storage; // Storage for items

    void logUpgradeDetails() const; // Helper for upgrade debug logs

public:
    explicit House(const sf::Texture& tex, int initialLevel = 1);

    // Getters
    const std::unordered_map<std::string, int>& getStorage() const;
    int getLevel() const { return level; }
    int getMaxStorageCapacity() const { return maxStorageCapacity; }
    float getEnergyRegenRate() const { return energyRegenRate; }
    float getUpgradeCost() const; 
    // Actions
    void regenerateEnergy(NPCEntity& npc);  // Energy regeneration for NPC
    bool storeItem(const std::string& item, int quantity); // Store resources
    bool takeFromStorage(const std::string& item, int quantity, NPCEntity& npc); // Take resources
    bool upgrade(float& npcMoney, NPCEntity& npc); // Upgrade house level

    // Stats and Logs
    void displayStorage() const; // Display storage details
    void displayStats() const;   // Display house stats

    // AI Integration
    bool isStorageFull() const;  // Check if storage is full
    bool isUpgradeAvailable(float npcMoney) const; // Check if NPC can afford an upgrade
    int getStoredItemCount(const std::string& item) const; // Get quantity of a specific resource

    // Rendering
    void draw(sf::RenderWindow& window) override; // Render house
    ObjectType getType() const override;         // Return object type
};

#endif // HOUSE_HPP
