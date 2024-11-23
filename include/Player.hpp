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
    std::string name;

public:
    PlayerEntity(const std::string& playerName, float initHealth, float initHunger, float initEnergy, float initSpeed, float initStrength, float initMoney)
        : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney), name(playerName) {}

    const std::string& getName() const { return name; } // Getter for the name of the NPC

    void handleInput(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move(0, -1, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move(0, 1, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move(-1, 0, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move(1, 0, deltaTime);
    }

    // Setters for health, strength, and speed
    void setHealth(float newHealth) {
        health = std::clamp(newHealth, 0.0f, 100.0f); // Ensure health stays within 0-100
        getDebugConsole().logOnce("Player", "Health set to: " + std::to_string(health));
    }

    void setStrength(float newStrength) {
        strength = std::max(0.0f, newStrength); // Ensure strength is non-negative
        getDebugConsole().logOnce("Player", "Strength set to: " + std::to_string(strength));
    }

    void setSpeed(float newSpeed) {
        speed = std::max(0.0f, newSpeed); // Ensure speed is non-negative
        getDebugConsole().logOnce("Player", "Speed set to: " + std::to_string(speed)); // Throttled log
    }

    // Inventory management
    bool addToInventory(const std::string& item, int quantity) {
        int currentTotal = 0;
        for (const auto& [key, count] : inventory) currentTotal += count;

        if (currentTotal + quantity > inventoryCapacity) {
            getDebugConsole().logOnce("Inventory", "Inventory full! Cannot add " + item + ".");
            return false;
        }

        inventory[item] += quantity;
        getDebugConsole().log("Inventory", "Added " + std::to_string(quantity) + " " + item + "(s) to inventory.");
        return true;
    }


    bool removeFromInventory(const std::string& item, int quantity) {
        auto it = inventory.find(item);
        if (it != inventory.end() && it->second >= quantity) {
            it->second -= quantity;
            if (it->second == 0) inventory.erase(it);
            getDebugConsole().log("Inventory", "Removed " + std::to_string(quantity) + " " + item + "(s) from inventory.");
            return true;
        }

        getDebugConsole().logOnce("Inventory", "Failed to remove " + std::to_string(quantity) + " " + item + ".");
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
        getDebugConsole().log("Player", oss.str());
    }

    void setMoney(float newMoney) {
        money = newMoney;
        getDebugConsole().logOnce("Player", "Money updated to: " + std::to_string(money)); 
    }

    const std::unordered_map<std::string, int>& getInventory() const { return inventory; }
};

#endif
