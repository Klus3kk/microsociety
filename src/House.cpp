#include "House.hpp"

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

// Regenerate energy for the player
void House::regenerateEnergy(PlayerEntity& player) {
    float oldEnergy = player.getEnergy();
    player.setEnergy(std::min(player.getEnergy() + energyRegenRate, 100.0f));
    getDebugConsole().log("House", "Energy regenerated from " + std::to_string(oldEnergy) +
                                     " to " + std::to_string(player.getEnergy()));
}

// Store an item in the house
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
    getDebugConsole().log("House", "Stored " + std::to_string(quantity) + " " + item +
                                     "(s) in the house.");
    return true;
}

// Upgrade the house
bool House::upgrade(int& playerMoney) {
    int upgradeCost = level * 50;
    if (playerMoney >= upgradeCost) {
        playerMoney -= upgradeCost;
        level++;
        maxStorageCapacity += 10;
        energyRegenRate += 1.0f;
        healthBonus += 5;
        strengthBonus += 2;
        speedBonus += 1;

        getDebugConsole().log("House", "Upgraded to level " + std::to_string(level) +
                                     ". Storage: " + std::to_string(maxStorageCapacity) +
                                     ", Energy Regen: " + std::to_string(energyRegenRate) +
                                     ", Health Bonus: " + std::to_string(healthBonus) +
                                     ", Strength Bonus: " + std::to_string(strengthBonus) +
                                     ", Speed Bonus: " + std::to_string(speedBonus));
        return true;
    }
    getDebugConsole().log("House", "Not enough money to upgrade.");
    return false;
}

// Apply stat bonuses while inside the house
void House::applyStatBonuses(PlayerEntity& player) {
    player.setHealth(player.getHealth() + healthBonus);
    player.setStrength(player.getStrength() + strengthBonus);
    player.setSpeed(player.getSpeed() + speedBonus);
    getDebugConsole().log("House", "Applied stat bonuses: Health +" + std::to_string(healthBonus) +
                                     ", Strength +" + std::to_string(strengthBonus) +
                                     ", Speed +" + std::to_string(speedBonus));
}

// Remove stat bonuses when leaving the house
void House::removeStatBonuses(PlayerEntity& player) {
    player.setHealth(player.getHealth() - healthBonus);
    player.setStrength(player.getStrength() - strengthBonus);
    player.setSpeed(player.getSpeed() - speedBonus);
    getDebugConsole().log("House", "Removed stat bonuses: Health -" + std::to_string(healthBonus) +
                                     ", Strength -" + std::to_string(strengthBonus) +
                                     ", Speed -" + std::to_string(speedBonus));
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
