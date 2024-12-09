#ifndef HOUSE_HPP
#define HOUSE_HPP

#include "Object.hpp"
#include "Player.hpp"
#include <unordered_map>
#include <string>

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

    const std::unordered_map<std::string, int>& getStorage() const;

    // Energy regeneration
    void regenerateEnergy(PlayerEntity& player);

    // Store item in the house
    bool storeItem(const std::string& item, int quantity);

    // Take item from the storage
    bool takeFromStorage(const std::string& item, int quantity, PlayerEntity& npc);

    // Upgrade the house
    bool upgrade(float& playerMoney, PlayerEntity& player);

    // Display the storage contents
    void displayStorage() const;

    // Display house stats
    void displayStats() const;

    // Override draw method
    void draw(sf::RenderWindow& window) override;

    // Get object type
    ObjectType getType() const override;
};

#endif // HOUSE_HPP
