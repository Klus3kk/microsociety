#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "debug.hpp"
#include <SFML/Window.hpp>
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>

class PlayerEntity : public Entity {
private:
    std::unordered_map<std::string, int> inventory; // Map for items and their quantities
    int inventoryCapacity = 10;                     // Max inventory capacity
    std::string name;                               // Player's name
    float baseSpeed = 150.0f;                       // Default base speed
    float currentSpeed = baseSpeed;                 // Current movement speed

public:
    // Constructor
    PlayerEntity(const std::string& playerName, float initHealth, float initHunger, float initEnergy,
                 float initSpeed, float initStrength, float initMoney)
        : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney), 
          name(playerName) {}

    // Getters
    const std::string& getName() const { return name; }
    float getBaseSpeed() const { return baseSpeed; }
    float getMaxEnergy() const { return 100.0f; }
    float getEnergyPercentage() const { return energy / getMaxEnergy(); }
    const std::unordered_map<std::string, int>& getInventory() const { return inventory; }
    int getMaxInventorySize() const { return inventoryCapacity; }
    int getInventorySize() const {
        int total = 0;
        for (const auto& [key, count] : inventory) total += count;
        return total;
    }
    int getInventoryItemCount(const std::string& item) const {
        auto it = inventory.find(item);
        return it != inventory.end() ? it->second : 0;
    }

    // Movement and Input
    void handleInput(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move(0, -1, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move(0, 1, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move(-1, 0, deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move(1, 0, deltaTime);
    }

    // Inventory Management
    bool addToInventory(const std::string& item, int quantity) {
        int totalItems = getInventorySize();
        if (totalItems + quantity > inventoryCapacity) {
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

    void displayInventory() const {
        std::ostringstream oss;
        oss << "Inventory Contents:\n";
        for (const auto& [item, quantity] : inventory) {
            oss << "- " << item << ": " << quantity << "\n";
        }
        getDebugConsole().log("Inventory", oss.str());
    }

    // Stat Modifiers
    void setHealth(float newHealth) {
        health = std::clamp(newHealth, 0.0f, 100.0f);
        getDebugConsole().logOnce("Player", "Health set to: " + std::to_string(health));
    }

    void setStrength(float newStrength) {
        strength = std::max(0.0f, newStrength);
        getDebugConsole().logOnce("Player", "Strength set to: " + std::to_string(strength));
    }

    void setSpeed(float newSpeed) {
        currentSpeed = std::max(0.0f, newSpeed);
    }

    void setMoney(float newMoney) {
        money = newMoney;
        getDebugConsole().logOnce("Player", "Money updated to: " + std::to_string(money));
    }

    // Energy Management
    void consumeEnergy(float amount) {
        energy = std::max(0.0f, energy - amount);
        if (energy == 0.0f) {
            getDebugConsole().logOnce("Player", "Player has no energy and must rest!");
        }
    }

    void regenerateEnergy(float rate) {
        if (energy < getMaxEnergy()) {
            energy = std::min(getMaxEnergy(), energy + rate);
            getDebugConsole().logOnce("Player", "Player's energy regenerated to " + std::to_string(energy));
        }
    }
};

#endif
