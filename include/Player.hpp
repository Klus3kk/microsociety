#ifndef PLAYERENTITY_HPP
#define PLAYERENTITY_HPP

#include "Entity.hpp"
#include <SFML/Window.hpp>

class PlayerEntity : public Entity {
public:
    PlayerEntity(float initHealth, float initHunger, float initEnergy, float initSpeed, float initStrength, float initMoney)
        : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney) {}

    // Handling user input for movement
    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            move(0, -1);  // Move up
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            move(0, 1);   // Move down
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            move(-1, 0);  // Move left
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            move(1, 0);   // Move right
        }
    }
};

#endif // PLAYERENTITY_HPP
