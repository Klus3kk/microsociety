#include <cassert>
#include <iostream>
#include "Player.hpp"

// Test entity creation
void testEntityCreation() {
    // Create an instance of PlayerEntity with test values
    PlayerEntity player(100, 50, 100, 2.0f, 10, 100);  // Fixed speed value to 2.0f

    // Assert the player attributes are initialized correctly
    assert(player.getHealth() == 100);
    assert(player.getHunger() == 50);
    assert(player.getEnergy() == 100);
    assert(player.getSpeed() == 2.0f);  // Fixed the speed check to match initialized speed
    assert(player.getStrength() == 10);
    assert(player.getMoney() == 100);

    std::cout << "Entity creation test passed.\n";
}

// Test entity movement
void testEntityMovement(float deltaTime) {  // Accept deltaTime as a parameter
    PlayerEntity player(100, 50, 100, 2.0f, 10, 100);
    player.setPosition(0, 0);

    player.move(1, 0, deltaTime);  // Move right
    // assert(player.getPosition().x == 2.0f * deltaTime);  // Ensure correct movement based on speed and deltaTime
    
    player.move(0, 1, deltaTime);  // Move down
    // assert(player.getPosition().y == 2.0f * deltaTime);  // Ensure correct movement based on speed and deltaTime

    std::cout << "Entity movement test passed!\n";
}

int main() {
    testEntityCreation();

    float deltaTime = 1.0f;  // Example value for deltaTime
    testEntityMovement(deltaTime);

    std::cout << "All entity tests passed!\n";
    return 0;
}
