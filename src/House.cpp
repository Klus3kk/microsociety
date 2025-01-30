#include "House.hpp"
#include "debug.hpp"
#include <sstream>
#include "Configuration.hpp"

// Constructor: Initializes house properties based on level
House::House(const sf::Texture& tex, int initialLevel)
    : level(initialLevel),
      maxStorageCapacity(initialLevel * 10),      // Storage capacity increases with level
      energyRegenRate(initialLevel * 2.0f),      // Energy regeneration rate scales with level
      healthBonus(initialLevel * 5),            // Health bonus per level
      strengthBonus(initialLevel * 2),          // Strength bonus per level
      speedBonus(initialLevel * 1) {            // Speed bonus per level
    texture = tex;
    sprite.setTexture(texture);
}

// Getters for house storage
const std::unordered_map<std::string, int>& House::getStorage() const {
    return storage;
}

// Resource requirements for upgrades
int House::getWoodRequirement() const {
    return GameConfig::BASE_WOOD_REQUIREMENT + level - 1; 
}

int House::getStoneRequirement() const {
    return GameConfig::BASE_STONE_REQUIREMENT + level - 1;
}

int House::getBushRequirement() const {
    return GameConfig::BASE_BUSH_REQUIREMENT + level - 1;
}

// Get required amount of a specific resource
int House::getRequirementForItem(const std::string& item) const {
    if (item == "wood") return getWoodRequirement();
    if (item == "stone") return getStoneRequirement();
    if (item == "bush") return getBushRequirement();
    return 0;  // Default case if item isn't a requirement
}

// Regenerate NPC energy inside the house
void House::regenerateEnergy(NPCEntity& npc) {
    if (npc.getEnergy() < npc.getMaxEnergy()) {
        npc.regenerateEnergy(10.0f); // Restores 10 energy per tick
        npc.restoreHealth(5.0f);     // Restores 5 health per tick
        getDebugConsole().log("House", npc.getName() + " is recovering energy at home.");
    }
}

// Store item in the house's storage
bool House::storeItem(const std::string& item, int quantity) {
    int currentTotal = std::accumulate(storage.begin(), storage.end(), 0,
                                       [](int sum, const auto& pair) { return sum + pair.second; });

    if (currentTotal + quantity > maxStorageCapacity) {
        getDebugConsole().log("House", "Storage full! Selling excess " + item + ".");
        return false; // NPC should sell instead
    }

    storage[item] += quantity;
    getDebugConsole().log("House", "Stored " + std::to_string(quantity) + " " + item + "(s).");
    return true;
}

// Retrieve item from storage
bool House::takeFromStorage(const std::string& item, int quantity, NPCEntity& npc) {
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

// Upgrade the house if NPC has enough resources
bool House::upgrade(float& npcMoney, NPCEntity& npc) {
    float upgradeCost = getUpgradeCost();
    int woodRequired = getWoodRequirement();
    int stoneRequired = getStoneRequirement();
    int bushRequired = getBushRequirement();

    if (npcMoney >= upgradeCost &&
        getStoredItemCount("wood") >= woodRequired &&
        getStoredItemCount("stone") >= stoneRequired &&
        getStoredItemCount("bush") >= bushRequired) {

        npcMoney -= upgradeCost;

        // Reduce required resources from storage
        if (storage.count("wood")) storage["wood"] -= woodRequired;
        if (storage.count("stone")) storage["stone"] -= stoneRequired;
        if (storage.count("bush")) storage["bush"] -= bushRequired;

        // Upgrade stats
        level++;
        maxStorageCapacity += 15;
        energyRegenRate += 2.0f;
        healthBonus += 10;
        strengthBonus += 3;
        speedBonus += 2;

        // Apply upgrades to NPC
        npc.setHealth(npc.getHealth() + healthBonus);
        npc.setStrength(npc.getStrength() + strengthBonus);
        npc.setSpeed(npc.getSpeed() + speedBonus);

        logUpgradeDetails();
        return true;
    }

    getDebugConsole().log("House", npc.getName() + " Upgrade failed due to insufficient resources.");
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

// Calculate the cost for the next upgrade
float House::getUpgradeCost() const {
    return GameConfig::BASE_UPGRADE_COST + (50 * std::pow(level, 1.5f));
}

// Display house storage contents
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

// Render the house on the screen
void House::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Get the type of this object (House)
ObjectType House::getType() const {
    return ObjectType::House;
}

// Check if storage is full
bool House::isStorageFull() const {
    int currentTotal = std::accumulate(storage.begin(), storage.end(), 0,
                                       [](int sum, const auto& pair) { return sum + pair.second; });
    return currentTotal >= maxStorageCapacity;
}

// Check if an upgrade is available for NPC
bool House::isUpgradeAvailable(float npcMoney) const {
    float upgradeCost = getUpgradeCost();

    return npcMoney >= upgradeCost &&
           getStoredItemCount("wood") >= getWoodRequirement() &&
           getStoredItemCount("stone") >= getStoneRequirement() &&
           getStoredItemCount("bush") >= getBushRequirement();
}

// Get the count of a specific stored item
int House::getStoredItemCount(const std::string& item) const {
    auto it = storage.find(item);
    return it != storage.end() ? it->second : 0;
}
