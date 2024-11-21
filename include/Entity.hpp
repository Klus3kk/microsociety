#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Configuration.hpp"
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
        : health(initHealth), hunger(initHunger), energy(initEnergy), speed(initSpeed),
          strength(initStrength), money(initMoney) {
    }

    // destructor
    virtual ~Entity() = default;

    // function to set texture for the entity
    void setTexture(const sf::Texture& tex, const sf::Color& color = sf::Color::White) {
        texture = tex;
        sprite.setTexture(texture);
        sprite.setColor(color); // Set the color directly
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
    // function for getting the current sprite of the entity
    const sf::Sprite& getSprite() const {
        return sprite;
    }


    // function to draw the entity
    void draw(sf::RenderWindow &window) {
        window.draw(sprite);
    }

    // accessors
    float getHealth() const { return health; }
    float getHunger() const { return hunger; }
    float getEnergy() const { return energy; }
    float getSpeed() const { return speed; }
    float getStrength() const { return strength; }
    float getMoney() const { return money; }

    // setters
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setEnergy(float newEnergy) { energy = newEnergy; } 
    void setMoney(float newMoney) { money = newMoney; }

    // movement function based on screen's framerate (cool, that was wrong for 2 months)
    void move(float dx, float dy, float deltaTime) {
        float newX = position.x + dx * speed * deltaTime;
        float newY = position.y + dy * speed * deltaTime;

        // Logical bounds for movement, configurable via GameConfig
        const float minX = -10.0f; // Left edge boundary
        const float minY = -8.0f;  // Top edge boundary
        const float maxX = GameConfig::mapWidth * GameConfig::tileSize - sprite.getGlobalBounds().width + 15.0f; // Right edge
        const float maxY = GameConfig::mapHeight * GameConfig::tileSize - sprite.getGlobalBounds().height + 6.0f; // Bottom edge

        // Clamp the new position to these bounds
        newX = std::clamp(newX, minX, maxX);
        newY = std::clamp(newY, minY, maxY);

        // Update position and sprite
        position = {newX, newY};
        sprite.setPosition(position);
    }



    // function for cheching collision
    bool checkCollision(const Entity& other) const {
        return sprite.getGlobalBounds().intersects(other.getSprite().getGlobalBounds());
    }

};

#endif 
