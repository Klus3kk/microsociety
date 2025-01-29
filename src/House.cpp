#include "House.hpp"
#include "debug.hpp"
#include <sstream>
#include "Configuration.hpp"

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
}

// Getters
const std::unordered_map<std::string, int>& House::getStorage() const {
    return storage;
}

int House::getWoodRequirement() const {
    return GameConfig::BASE_WOOD_REQUIREMENT + level - 1; 
}

int House::getStoneRequirement() const {
    return GameConfig::BASE_STONE_REQUIREMENT + level - 1;
}

int House::getBushRequirement() const {
    return GameConfig::BASE_BUSH_REQUIREMENT + level - 1;
}

int House::getRequirementForItem(const std::string& item) const {
    if (item == "wood") return getWoodRequirement();
    if (item == "stone") return getStoneRequirement();
    if (item == "bush") return getBushRequirement();
    return 0;  // Default case
}


// Regenerate NPC energy
void House::regenerateEnergy(NPCEntity& npc) {
    if (npc.getEnergy() < 100.0f) {
        npc.regenerateEnergy(energyRegenRate * 1.5f); // 50% faster regen
        getDebugConsole().log("House", npc.getName() + "'s energy regenerated.");
    }

    if (npc.getEnergy() > 100.0f) {
        npc.setEnergy(100.0f);
        getDebugConsole().log("House", npc.getName() + "'s energy capped at max.");
    }
}

// Store item in the house
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


// Take item from storage
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

        // Reduce only if resource exists
        if (storage.count("wood")) storage["wood"] -= woodRequired;
        if (storage.count("stone")) storage["stone"] -= stoneRequired;
        if (storage.count("bush")) storage["bush"] -= bushRequired;

        level++;
        maxStorageCapacity += 15;
        energyRegenRate += 2.0f;
        healthBonus += 10;
        strengthBonus += 3;
        speedBonus += 2;

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

float House::getUpgradeCost() const {
    return GameConfig::BASE_UPGRADE_COST + (50 * std::pow(level, 1.5f));
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
