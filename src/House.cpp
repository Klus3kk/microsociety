#include "House.hpp"
#include "debug.hpp"
#include <sstream>

// Constructor
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

// Getters
const std::unordered_map<std::string, int>& House::getStorage() const {
    return storage;
}

// Regenerate NPC energy
void House::regenerateEnergy(PlayerEntity& npc) {
    if (npc.getEnergy() < 100.0f) { // Use 100.0f as the maximum energy
        npc.regenerateEnergy(energyRegenRate); // Adjusted regeneration rate
        getDebugConsole().log("House", npc.getName() + "'s energy regenerated by " + std::to_string(energyRegenRate) + ".");
    }

    if (npc.getEnergy() > 100.0f) { // Prevent over-regeneration
        npc.setEnergy(100.0f);
        getDebugConsole().log("House", npc.getName() + "'s energy capped at maximum.");
    }
}

// Store item in the house
bool House::storeItem(const std::string& item, int quantity) {
    int currentTotal = std::accumulate(storage.begin(), storage.end(), 0,
                                       [](int sum, const auto& pair) { return sum + pair.second; });

    if (currentTotal + quantity > maxStorageCapacity) {
        getDebugConsole().log("House", "Storage full! Could not store " + item + ".");
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
bool House::upgrade(float& npcMoney, PlayerEntity& npc) {
    int upgradeCost = level * 50 + 25; // Dynamic cost scaling
    int woodRequired = level * 15;    // Linear scaling
    int stoneRequired = level * 10;
    int bushRequired = level * 5;     // Added bushes as a requirement

    // Check if the NPC has enough money and resources
    if (npcMoney >= upgradeCost &&
        storage["wood"] >= woodRequired &&
        storage["stone"] >= stoneRequired &&
        storage["bush"] >= bushRequired) {
        
        npcMoney -= upgradeCost;
        storage["wood"] -= woodRequired;
        storage["stone"] -= stoneRequired;
        storage["bush"] -= bushRequired; // Deduct bushes
        level++;
        maxStorageCapacity += 10;
        energyRegenRate += 1.0f;
        healthBonus += 5;
        strengthBonus += 2;
        speedBonus += 1;

        // Update NPC stats
        npc.setHealth(npc.getHealth() + healthBonus);
        npc.setStrength(npc.getStrength() + strengthBonus);
        npc.setSpeed(npc.getSpeed() + speedBonus);

        logUpgradeDetails();
        return true;
    }

    // Log missing requirements
    if (npcMoney < upgradeCost) {
        getDebugConsole().log("House", npc.getName() + " needs " + std::to_string(upgradeCost - npcMoney) + " more money.");
    }
    if (storage["wood"] < woodRequired) {
        getDebugConsole().log("House", "You need " + std::to_string(woodRequired - storage["wood"]) + " more wood.");
    }
    if (storage["stone"] < stoneRequired) {
        getDebugConsole().log("House", "You need " + std::to_string(stoneRequired - storage["stone"]) + " more stone.");
    }
    if (storage["bush"] < bushRequired) {
        getDebugConsole().log("House", "You need " + std::to_string(bushRequired - storage["bush"]) + " more bush(es).");
    }

    return false;
}

// Log upgrade details
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

// Check if storage is full
bool House::isStorageFull() const {
    int currentTotal = std::accumulate(storage.begin(), storage.end(), 0,
                                       [](int sum, const auto& pair) { return sum + pair.second; });
    return currentTotal >= maxStorageCapacity;
}

// Check if an upgrade is available
bool House::isUpgradeAvailable(float npcMoney) const {
    int upgradeCost = level * 50 + 25;
    return npcMoney >= upgradeCost &&
           storage["wood"] >= level * 15 &&
           storage["stone"] >= level * 10 &&
           storage["bush"] >= level * 5;
}

// Get the count of a specific stored item
int House::getStoredItemCount(const std::string& item) const {
    auto it = storage.find(item);
    return it != storage.end() ? it->second : 0;
}
