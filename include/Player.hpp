#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include <SFML/Window.hpp>
#include <unordered_map>

class PlayerEntity : public Entity {
private:
    std::unordered_map<std::string, int> inventory; // Item, quantity
    int inventoryCapacity = 10;

public:
    PlayerEntity(float initHealth, float initHunger, float initEnergy, float initSpeed, float initStrength, float initMoney)
    : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney) {}

    void handleInput(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move(0, -1, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move(0, 1, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move(-1, 0, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move(1, 0, deltaTime);
    }

    bool addToInventory(const std::string& item, int quantity) {
        int currentTotal = 0;
        for (const auto& [key, count] : inventory) currentTotal += count;
        if (currentTotal + quantity > inventoryCapacity) {
            std::cout << "Inventory full!\n";
            return false;
        }
        inventory[item] += quantity;
        return true;
    }

    int getInventoryItemCount(const std::string& item) const {
        auto it = inventory.find(item);
        return it != inventory.end() ? it->second : 0;
    }

    void displayInventory() const {
        for (const auto& [item, quantity] : inventory) std::cout << item << ": " << quantity << "\n";
    }

    void setMoney(float newMoney) {
        money = newMoney;
    }

    const std::unordered_map<std::string, int>& getInventory() const { return inventory; }
};

#endif 
