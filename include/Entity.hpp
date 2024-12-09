#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Configuration.hpp"

enum class AnimationState { Idle, Walking, Interacting };

class Entity {
protected:
    float health;
    float hunger;
    float energy;
    float speed;
    float strength;
    float money;
    sf::Vector2f position;
    sf::Sprite sprite;
    sf::Texture texture;
    AnimationState state = AnimationState::Idle;

public:
    Entity(float initHealth, float initHunger, float initEnergy, float initSpeed, float initStrength, float initMoney)
        : health(initHealth), hunger(initHunger), energy(initEnergy), speed(initSpeed), strength(initStrength), money(initMoney) {}

    virtual ~Entity() = default;

    // Texture and rendering
    void setTexture(const sf::Texture& tex, const sf::Color& color = sf::Color::White) {
        texture = tex;
        sprite.setTexture(texture);
        sprite.setColor(color);
    }

    void setPosition(float x, float y) {
        position = {x, y};
        sprite.setPosition(position);
    }

    void setSize(float scaleX, float scaleY) {
        sprite.setScale(scaleX, scaleY);
    }

    sf::Vector2f getPosition() const { return position; }
    const sf::Sprite& getSprite() const { return sprite; }

    // Accessors
    float getHealth() const { return health; }
    float getHunger() const { return hunger; }
    float getEnergy() const { return energy; }
    float getSpeed() const { return speed; }
    float getStrength() const { return strength; }
    float getMoney() const { return money; }

    // Modifiers
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setEnergy(float newEnergy) { energy = newEnergy; }
    void setMoney(float newMoney) { money = newMoney; }

    // Movement with boundary checking
    void move(float dx, float dy, float deltaTime) {
        float newX = position.x + dx * speed * deltaTime;
        float newY = position.y + dy * speed * deltaTime;

        // Logical bounds for movement
        const float minX = 0.0f;
        const float minY = 0.0f;
        const float maxX = GameConfig::mapWidth * GameConfig::tileSize - sprite.getGlobalBounds().width;
        const float maxY = GameConfig::mapHeight * GameConfig::tileSize - sprite.getGlobalBounds().height;

        // Clamp position within bounds
        position.x = std::clamp(newX, minX, maxX);
        position.y = std::clamp(newY, minY, maxY);
        sprite.setPosition(position);
    }

    // Collision detection
    bool checkCollision(const Entity& other) const {
        return sprite.getGlobalBounds().intersects(other.getSprite().getGlobalBounds());
    }

    // Rendering
    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

#endif // ENTITY_HPP
