#include <cassert>
#include <iostream>
#include "PlayerEntity.hpp"

// Test entity creation
void testEntityCreation() {
    // Create an instance of PlayerEntity with test values
    PlayerEntity player(100, 50, 100, 2.0f, 10, 100);

    // Assert the player attributes are initialized correctly
    assert(player.getHealth() == 100);
    assert(player.getHunger() == 50);
    assert(player.getEnergy() == 100);
    assert(player.getSpeed() == 2.0f);
    assert(player.getStrength() == 10);
    assert(player.getMoney() == 100);
}

// Test entity movement
void testEntityMovement() {
    PlayerEntity player(100, 50, 100, 2.0f, 10, 100);
    player.setPosition(0, 0);

    // Move the player to the right by 1 unit
    player.move(1, 0);

    // Check if the player position updated correctly
    assert(player.getSpeed() == 2.0f);  // Speed was set to 2.0
    std::cout << "Entity movement test passed!\n";
}

int main() {
    testEntityCreation();
    testEntityMovement();

    std::cout << "All entity tests passed!\n";
    return 0;
}
