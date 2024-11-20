#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "debug.hpp" 
#include <SFML/Window.hpp>
#include <unordered_map>
#include <string>

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
            getDebugConsole().log("Inventory full! Cannot add " + item);
            return false;
        }

        inventory[item] += quantity;
        getDebugConsole().log("Added " + std::to_string(quantity) + " " + item + "(s) to inventory.");
        return true;
    }

    bool removeFromInventory(const std::string& item, int quantity) {
        auto it = inventory.find(item);
        if (it != inventory.end() && it->second >= quantity) {
            it->second -= quantity;
            if (it->second == 0) inventory.erase(it);
            getDebugConsole().log("Removed " + std::to_string(quantity) + " " + item + "(s) from inventory.");
            return true;
        }

        getDebugConsole().log("Failed to remove " + std::to_string(quantity) + " " + item + "(s) from inventory.");
        return false;
    }

    int getInventorySize() const {
        int currentTotal = 0;
        for (const auto& [key, count] : inventory) currentTotal += count;
        return currentTotal;
    }

    int getMaxInventorySize() const {
        return inventoryCapacity;
    }

    int getInventoryItemCount(const std::string& item) const {
        auto it = inventory.find(item);
        return it != inventory.end() ? it->second : 0;
    }

    void displayInventory() const {
        std::ostringstream oss;
        oss << "Inventory Contents:\n";
        for (const auto& [item, quantity] : inventory) {
            oss << "- " << item << ": " << quantity << "\n";
        }
        getDebugConsole().log(oss.str());
    }

    void setMoney(float newMoney) {
        money = newMoney;
        getDebugConsole().log("Money updated to: " + std::to_string(money));
    }

    const std::unordered_map<std::string, int>& getInventory() const { return inventory; }
};

#endif
