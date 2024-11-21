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

    // Display storage contents
    void displayStorage() const;

    // Draw method override
    void draw(sf::RenderWindow& window) override;
};

#endif 
