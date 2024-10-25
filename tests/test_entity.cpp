#include <cassert>
#include <iostream>
#include "Player.hpp"

// test entity creation
void testEntityCreation() {
    // creating PlayerEntity with test values, to check assigning values
    PlayerEntity player(100, 50, 100, 2.0f, 10, 100); 

    // checking if the player attributes are initialized correctly
    assert(player.getHealth() == 100);
    assert(player.getHunger() == 50);
    assert(player.getEnergy() == 100);
    assert(player.getSpeed() == 2.0f);  
    assert(player.getStrength() == 10);
    assert(player.getMoney() == 100);

    std::cout << "Entity creation test passed.\n";
}

// test entity movement
void testEntityMovement(float deltaTime) { 
    PlayerEntity player(100, 50, 100, 2.0f, 10, 100);
    player.setPosition(0, 0);

    player.move(1, 0, deltaTime);  
    assert(player.getPosition().x == 2.0f * deltaTime);  // ensure correct movement based on speed and deltaTime
    
    player.move(0, 1, deltaTime);  
    assert(player.getPosition().y == 2.0f * deltaTime);  

    std::cout << "Entity movement test passed!\n";
}

int main() {
    testEntityCreation();

    float deltaTime = 1.0f;  
    testEntityMovement(deltaTime);

    std::cout << "All entity tests passed!\n";
    return 0;
}
