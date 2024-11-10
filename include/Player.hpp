#ifndef PLAYER_HPP
#define PLAYER_HPP


#include "Entity.hpp"
#include <SFML/Window.hpp>

class PlayerEntity : public Entity {
public:
    PlayerEntity(float initHealth, float initHunger, float initEnergy, float initSpeed, float initStrength, float initMoney)
    : Entity(initHealth, initHunger, initEnergy, initSpeed, initStrength, initMoney) {}

    // handling user input for movement
    void handleInput(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            move(0, -1, deltaTime);  // up
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            move(0, 1, deltaTime);   // down
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            move(-1, 0, deltaTime);  // left
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            move(1, 0, deltaTime);   // right
        }
    }
};

#endif 
