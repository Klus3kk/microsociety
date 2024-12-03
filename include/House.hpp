#ifndef HOUSE_HPP
#define HOUSE_HPP

#include "Object.hpp"
#include "Player.hpp"
#include <unordered_map>
#include <iostream>

class House : public Object {
private:
    int level;
    int maxStorageCapacity;
    float energyRegenRate;
    int healthBonus;
    int strengthBonus;
    int speedBonus;
    std::unordered_map<std::string, int> storage;

public:
    House(const sf::Texture& tex, int initialLevel = 1);

    const std::unordered_map<std::string, int>& getStorage() const { return storage; }

    // Energy regeneration
    void regenerateEnergy(PlayerEntity& player);

    // Store item in the house
    bool storeItem(const std::string& item, int quantity);

    // Upgrade house
    bool upgrade(int& playerMoney);

    // Apply stat bonuses to the player while inside the house
    void applyStatBonuses(PlayerEntity& player);

    // Remove stat bonuses when leaving the house
    void removeStatBonuses(PlayerEntity& player);

    // Display the storage contents
    void displayStorage() const;

    // Taking from storage 
    bool takeFromStorage(const std::string& item, int quantity, PlayerEntity& npc);


    // Display house stats
    void displayStats() const;

    // Draw method override
    void draw(sf::RenderWindow& window) override;

    ObjectType getType() const override;
};

#endif
