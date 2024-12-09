#include "House.hpp"
#include "debug.hpp"
#include <sstream>

House::House(const sf::Texture& tex, int initialLevel)
    : level(initialLevel),
      maxStorageCapacity(initialLevel * 10),
      energyRegenRate(initialLevel * 2.0f),
      healthBonus(initialLevel * 5),
      strengthBonus(initialLevel * 2),
      speedBonus(initialLevel * 1) {
    texture = tex;
    sprite.setTexture(texture);

    // Default test items in storage
    storage["wood"] = 10;
    storage["stone"] = 5;
    storage["bush"] = 3;
}

const std::unordered_map<std::string, int>& House::getStorage() const {
    return storage;
}

// Regenerate player energy
void House::regenerateEnergy(PlayerEntity& player) {
    if (player.getEnergy() < 100.0f) { // Use 100.0f as the maximum energy
        player.regenerateEnergy(10.0f); // Adjust regeneration rate as needed
        getDebugConsole().log("House", "Player's energy regenerated by house.");
    }

    if (player.getEnergy() > 100.0f) { // Prevent over-regeneration
        player.setEnergy(100.0f);
        getDebugConsole().log("House", "Player's energy capped at maximum.");
    }
}

// Store item in the house
bool House::storeItem(const std::string& item, int quantity) {
    int currentTotal = 0;
    for (const auto& [key, value] : storage) {
        currentTotal += value;
    }

    if (currentTotal + quantity > maxStorageCapacity) {
        getDebugConsole().log("House", "Storage full! Could not store " + item);
        return false;
    }

    storage[item] += quantity;
    getDebugConsole().log("House", "Stored " + std::to_string(quantity) + " " + item + "(s) in the house.");
    return true;
}

// Take item from storage
bool House::takeFromStorage(const std::string& item, int quantity, PlayerEntity& npc) {
    auto it = storage.find(item);
    if (it != storage.end() && it->second >= quantity) {
        if (npc.getInventorySize() + quantity > npc.getMaxInventorySize()) {
            getDebugConsole().log("House", npc.getName() + " does not have enough inventory space.");
            return false;
        }
        npc.addToInventory(item, quantity);
        it->second -= quantity;

        if (it->second == 0) {
            storage.erase(it); // Remove the item if quantity reaches zero
        }

        getDebugConsole().log("House", npc.getName() + " took " + std::to_string(quantity) + " " + item + "(s).");
        return true;
    }

    getDebugConsole().log("House", "Not enough " + item + " in storage.");
    return false;
}

// Upgrade the house
bool House::upgrade(float& playerMoney, PlayerEntity& player) {
    int upgradeCost = level * 50;

    if (playerMoney >= upgradeCost) {
        playerMoney -= upgradeCost;
        level++;
        maxStorageCapacity += 10;
        energyRegenRate += 1.0f;
        healthBonus += 5;
        strengthBonus += 2;
        speedBonus += 1;

        player.setHealth(player.getHealth() + healthBonus);
        player.setStrength(player.getStrength() + strengthBonus);
        player.setSpeed(player.getSpeed() + speedBonus);

        logUpgradeDetails();
        return true;
    }

    getDebugConsole().log("House", "Not enough money to upgrade.");
    return false;
}

void House::logUpgradeDetails() const {
    getDebugConsole().log("House", "Upgraded to level " + std::to_string(level) +
                                         ". Storage: " + std::to_string(maxStorageCapacity) +
                                         ", Energy Regen: " + std::to_string(energyRegenRate) +
                                         ", Health Bonus: +" + std::to_string(healthBonus) +
                                         ", Strength Bonus: +" + std::to_string(strengthBonus) +
                                         ", Speed Bonus: +" + std::to_string(speedBonus));
}

// Display the storage contents
void House::displayStorage() const {
    std::ostringstream storageDisplay;
    storageDisplay << "House Storage (Capacity: " << maxStorageCapacity << "):\n";
    for (const auto& [item, quantity] : storage) {
        storageDisplay << "- " << item << ": " << quantity << "\n";
    }
    getDebugConsole().log("House", storageDisplay.str());
}

// Display house stats
void House::displayStats() const {
    std::ostringstream stats;
    stats << "House Stats:\n"
          << "- Level: " << level << "\n"
          << "- Storage Capacity: " << maxStorageCapacity << "\n"
          << "- Energy Regen Rate: " << energyRegenRate << "\n"
          << "- Health Bonus: " << healthBonus << "\n"
          << "- Strength Bonus: " << strengthBonus << "\n"
          << "- Speed Bonus: " << speedBonus << "\n";
    getDebugConsole().log("House", stats.str());
}

// Draw the house
void House::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Get the object type
ObjectType House::getType() const {
    return ObjectType::House;
}
