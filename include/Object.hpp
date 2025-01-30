#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics.hpp>

// Enum representing different types of objects that can exist in the game world
enum class ObjectType {
    None,   // No object
    Tree,   // Represents a tree
    Rock,   // Represents a rock
    Bush,   // Represents a bush
    House,  // Represents a house
    Market  // Represents a market
};

// Base class for all objects placed on tiles
class Object {
protected:
    sf::Sprite sprite;  // Sprite representing the object
    sf::Texture texture; // Texture applied to the sprite

public:
    virtual ~Object() = default; // Virtual destructor for polymorphism

    // Pure virtual functions (must be implemented by derived classes)
    virtual void draw(sf::RenderWindow& window) = 0;  // Render object
    virtual ObjectType getType() const = 0;           // Return object type

    // Getter for the sprite, allowing interaction with the object
    sf::Sprite& getSprite() {
        return sprite;
    }

    // Returns the bounding box with a slight offset for collision accuracy
    virtual sf::FloatRect getObjectBounds() const {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bounds.left -= 2;  // Slightly extend boundaries for better collision detection
        bounds.top -= 2;
        bounds.width += 4;
        bounds.height += 4;
        return bounds;
    }

    // Set the position of the object on the game map
    void setPosition(float x, float y) {
        sprite.setPosition(x, y); 
    }

    // Scale the object (useful for resizing assets)
    void setSize(float scaleX, float scaleY) {
        sprite.setScale(scaleX, scaleY);  
    }

    // Set the texture for the object (defined in the base class)
    void setTexture(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }
};

// Derived class representing a tree object
class Tree : public Object {
public:
    Tree(const sf::Texture& tex) {
        setTexture(tex); // Assign texture to the tree
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite); // Render tree on the screen
    }

    ObjectType getType() const override {
        return ObjectType::Tree; // Return tree type
    }
};

// Derived class representing a rock object
class Rock : public Object {
public:
    Rock(const sf::Texture& tex) {
        setTexture(tex); // Assign texture to the rock
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite); // Render rock on the screen
    }

    ObjectType getType() const override {
        return ObjectType::Rock; // Return rock type
    }
};

// Derived class representing a bush object
class Bush : public Object {
public:
    Bush(const sf::Texture& tex) {
        setTexture(tex); // Assign texture to the bush
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite); // Render bush on the screen
    }

    ObjectType getType() const override {
        return ObjectType::Bush; // Return bush type
    }
};

#endif 
