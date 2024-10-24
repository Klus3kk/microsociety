#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>

class Entity {
protected:
    float health;
    float hunger;
    float energy;
    float speed;
    float strength;
    float money;

    sf::Vector2f position;  // Position on the map
    sf::Sprite sprite;      // For rendering the NPC
    sf::Texture texture;    // NPC texture

public:
    // Constructor to initialize attributes
    Entity(float initHealth, float initHunger, float initEnergy, float initSpeed, float initStrength, float initMoney)
        : health(initHealth), hunger(initHunger), energy(initEnergy), speed(initSpeed), strength(initStrength), money(initMoney) {
    }

    // Destructor
    virtual ~Entity() = default;

    // Function to set texture for the entity
    void setTexture(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    // Function to set position of the entity
    void setPosition(float x, float y) {
        position = {x, y};
        sprite.setPosition(position);
    }

    // Function to draw the entity
    void draw(sf::RenderWindow &window) {
        window.draw(sprite);
        std::cout << "Drawing entity at position (" << position.x << ", " << position.y << "\n";
    }

    // Accessors
    float getHealth() const { return health; }
    float getHunger() const { return hunger; }
    float getEnergy() const { return energy; }
    float getSpeed() const { return speed; }
    float getStrength() const { return strength; }
    float getMoney() const { return money; }

    // Movement function (just an example for NPC movement)
    void move(float dx, float dy) {
        position.x += dx * speed;
        position.y += dy * speed;
        sprite.setPosition(position);
    }
};

#endif 
