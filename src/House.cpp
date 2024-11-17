#include "House.hpp"

House::House(const sf::Texture& tex, int initialLevel)
    : level(initialLevel), maxStorageCapacity(initialLevel * 10),
      energyRegenRate(initialLevel * 2.0f) {
    texture = tex;
    sprite.setTexture(texture);
}

// Regenerate energy for the player
void House::regenerateEnergy(PlayerEntity& player) {
    player.setEnergy(std::min(player.getEnergy() + energyRegenRate, 100.0f));
    std::cout << "Energy regenerated to " << player.getEnergy() << ".\n";
}

// Store an item in the house
bool House::storeItem(const std::string& item, int quantity) {
    int currentTotal = 0;
    for (const auto& [key, value] : storage) {
        currentTotal += value;
    }
    if (currentTotal + quantity > maxStorageCapacity) {
        std::cout << "House storage full!\n";
        return false;
    }
    storage[item] += quantity;
    std::cout << "Stored " << quantity << " " << item << " in the house.\n";
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
        std::cout << "House upgraded to level " << level << "!\n";
        return true;
    }
    std::cout << "Not enough money to upgrade the house.\n";
    return false;
}

// Display the storage contents
void House::displayStorage() const {
    std::cout << "House Storage:\n";
    for (const auto& [item, quantity] : storage) {
        std::cout << "- " << item << ": " << quantity << "\n";
    }
}

// Draw the house
void House::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
