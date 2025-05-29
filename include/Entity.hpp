#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Configuration.hpp"
#include <cfloat>
#include <cmath>

// // Defines different animation states for the entity
// enum class AnimationState { Idle, Walking, Interacting, Dead, Exhausted };

// Base class for all entities in the game
class Entity {
protected:
    float health;          // Current health of the entity
    float hunger;          // Hunger level affecting performance
    float energy;          // Energy level affecting movement/actions
    float speed;           // Movement speed of the entity
    float strength;        // Strength used in interactions (e.g., combat)
    float money;           // In-game currency
    sf::Vector2f position; // Current position on the map
    sf::Sprite sprite;     // Graphical representation
    sf::Texture texture;   // Texture associated with the sprite
    // AnimationState state = AnimationState::Idle; // Current animation state
    bool dead = false;     // Flag to track if the entity is dead

public:
    // Constructor to initialize core attributes
    Entity(float initHealth, float initHunger, float initEnergy, float initSpeed, float initStrength, float initMoney)
        : health(initHealth), hunger(initHunger), energy(initEnergy), speed(initSpeed), strength(initStrength), money(initMoney) {}

    virtual ~Entity() = default; // Virtual destructor ensures proper cleanup in derived classes

    // --- TEXTURE & RENDERING METHODS ---

    // Sets the texture and optional color overlay
    void setTexture(const sf::Texture& tex, const sf::Color& color = sf::Color::White) {
        if (!tex.getSize().x || !tex.getSize().y) {
            std::cerr << "Error: Invalid texture provided." << std::endl;
            sprite.setTexture(sf::Texture()); // Assign empty texture to avoid crashes
            return;
        }
        sprite.setTexture(tex);
        sprite.setColor(color);
    }

    // Sets the position of the entity on the map
    void setPosition(float x, float y) {
        position = {x, y};
        sprite.setPosition(position);
    }

    // Adjusts the entity's sprite scaling
    void setSize(float scaleX, float scaleY) {
        sprite.setScale(scaleX, scaleY);
    }

    // Getters for position and sprite
    sf::Vector2f getPosition() const { return position; }
    const sf::Sprite& getSprite() const { return sprite; }

    // --- ACCESSOR METHODS ---

    float getHealth() const { return health; }
    float getHunger() const { return hunger; }
    float getEnergy() const { return energy; }
    float getSpeed() const { return speed; }
    float getStrength() const { return strength; }

    // Ensures money value is always valid (no negative or NaN values)
    float getMoney() const { 
        if (std::isnan(money) || money < 0) {
            std::cerr << "ERROR: " << "Invalid money value detected for entity: " << money << std::endl;
            return 0.0f; 
        }
        return money; 
    }

    // AnimationState getState() const { return state; }
    bool isDead() const { return dead; }

    // --- MODIFIER METHODS ---

    void setHealth(float newHealth) { 
        health = std::clamp(newHealth, 0.0f, GameConfig::MAX_HEALTH);
        if (health <= 0.0f) {
            setDead(true);
        }
    }

    void setEnergy(float newEnergy) { 
        energy = std::clamp(newEnergy, 0.0f, GameConfig::MAX_ENERGY); 
    }

    void setHunger(float newHunger) { 
        hunger = std::clamp(newHunger, 0.0f, 100.0f); 
    }

    void setStrength(float newStrength) { 
        strength = std::max(0.0f, newStrength); 
    }

    void setSpeed(float newSpeed) { 
        speed = std::max(0.0f, newSpeed); 
    }

    void setDead(bool isDead) { 
        dead = isDead; 
        if (dead) {
            health = 0.0f;
            energy = 0.0f;
        }
    }
    
    // --- MOVEMENT SYSTEM ---

    // Moves the entity while ensuring it stays within map boundaries
    void move(float dx, float dy, float deltaTime) {
        if (dead) return; // Prevents movement if the entity is dead

        float newX = position.x + dx * speed * deltaTime;
        float newY = position.y + dy * speed * deltaTime;

        // Movement boundaries based on tile size and map dimensions
        const float minX = 0.0f;
        const float minY = 0.0f;
        const float maxX = GameConfig::mapWidth * GameConfig::tileSize - sprite.getGlobalBounds().width;
        const float maxY = GameConfig::mapHeight * GameConfig::tileSize - sprite.getGlobalBounds().height;

        // Ensures entity stays within valid bounds
        position.x = std::clamp(newX, minX, maxX);
        position.y = std::clamp(newY, minY, maxY);
        sprite.setPosition(position);
    }

    // --- COLLISION DETECTION ---

    // Checks if this entity collides with another entity
    bool checkCollision(const Entity& other) const {
        return sprite.getGlobalBounds().intersects(other.getSprite().getGlobalBounds());
    }

    // --- STATUS CHECKS ---

    // Determines if energy is low and needs regeneration
    bool needsEnergyRegeneration() const {
        return energy < 30.0f; // Threshold for low energy
    }

    // Determines if health is low and needs regeneration
    bool needsHealthRegeneration() const {
        return health < 50.0f; // Threshold for low health
    }

    // --- DAMAGE SYSTEM ---

    // Reduces health when entity takes damage, possibly killing it
    void takeDamage(float amount) {
        health = std::max(0.0f, health - amount);
        if (health == 0.0f) {
            // state = AnimationState::Dead;
            dead = true;
        }
    }

    // --- RENDERING ---

    // Draws the entity if it is not dead
    void draw(sf::RenderWindow& window) const {
        if (!dead) {
            window.draw(sprite);
        }
    }

    // --- REWARD & PENALTY SYSTEM ---

    // Virtual method for applying rewards (e.g., money for completing tasks)
    virtual void applyReward(int reward) {
        money += reward;
    }

    // Virtual method for applying penalties (e.g., losing money for failures)
    virtual void applyPenalty(int penalty) {
        money = std::max(0.0f, money - penalty);
    }


};

#endif
