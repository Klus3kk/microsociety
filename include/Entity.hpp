#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
class Entity {
protected:
    float health;
    float hunger;
    float energy;
    float speed;
    float strength;
    float money;

    sf::Vector2f position;  // position on the map
    sf::Sprite sprite;      // for rendering the NPC
    sf::Texture texture;    // Entity's texture

public:
    // constructor to initialize attributes
    Entity(float initHealth, float initHunger, float initEnergy, float initSpeed, float initStrength, float initMoney)
        : health(initHealth), hunger(initHunger), energy(initEnergy), speed(initSpeed), strength(initStrength), money(initMoney) {
    }

    // destructor
    virtual ~Entity() = default;

    // function to set texture for the entity
    void setTexture(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    // function to set position of the entity
    void setPosition(float x, float y) {
        position = {x, y};
        sprite.setPosition(position);
    }

    // function to set the size of the entity
    void setSize(float scaleX, float scaleY) {
        sprite.setScale(scaleX, scaleY);  
    }

    // function to get the current position of the entity
    sf::Vector2f getPosition() const {
        return position;
    }
    

    // function to draw the entity
    void draw(sf::RenderWindow &window) {
        window.draw(sprite);
        std::cout << "Drawing entity at position (" << position.x << ", " << position.y << ")\n";
    }

    // accessors
    float getHealth() const { return health; }
    float getHunger() const { return hunger; }
    float getEnergy() const { return energy; }
    float getSpeed() const { return speed; }
    float getStrength() const { return strength; }
    float getMoney() const { return money; }

    // movement function based on screen's framerate
    void move(float dx, float dy, float deltaTime) {
        position.x += dx * speed * deltaTime;
        position.y += dy * speed * deltaTime;
        sprite.setPosition(position);
        std::cout << "Entity moved to (" << position.x << ", " << position.y << ")\n";
    }
};

#endif 
