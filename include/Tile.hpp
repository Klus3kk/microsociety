#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>
#include "Object.hpp"
#include "debug.hpp"
#include <memory>

// Base class representing a tile in the game world
class Tile {
protected:
    sf::Sprite sprite; // The visual representation of the tile
    sf::Texture texture; // Texture used for rendering
    std::unique_ptr<Object> object; // Unique pointer to an object placed on the tile

public:
    Tile() = default; // Default constructor
    virtual ~Tile() = default; // Virtual destructor for proper polymorphic destruction

    // Sets the texture of the tile
    virtual void setTexture(const sf::Texture& tex) {
        texture = tex; // Store the texture
        sprite.setTexture(texture); // Apply texture to sprite
    }

    // Sets the position of the tile and aligns any placed object
    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
        if (object) {
            object->setPosition(x, y); // Ensure object follows tile's position
        }
    }

    // Draws the tile and any object on it
    virtual void draw(sf::RenderWindow& window) const {
        window.draw(sprite); // Draw the tile itself
        if (object) {
            object->draw(window); // Draw the placed object (if any)
        }
    }

    // Places an object on the tile
    void placeObject(std::unique_ptr<Object> obj) {
        object = std::move(obj); // Transfer ownership to unique_ptr
        if (object) {
            object->setPosition(sprite.getPosition().x, sprite.getPosition().y); // Align object position
        }
    }

    // Returns a raw pointer to the placed object (if any)
    Object* getObject() const {
        return object.get();
    }

    // Checks if there is an object on the tile
    bool hasObject() const {
        return object != nullptr;
    }

    // Retrieves the bounding box of the object on the tile (if any)
    sf::FloatRect getObjectBounds() const {
        return object ? object->getObjectBounds() : sf::FloatRect(); // Return empty bounds if no object
    }

    // Removes an object from the tile
    void removeObject() {
        if (object) {
            object.reset(); // Releases the unique_ptr, deleting the object
            getDebugConsole().log("Tile", "Object removed from tile.");
        }
    }

    // Gets the position of the tile
    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }
};

// Derived class representing a stone tile
class StoneTile : public Tile {
public:
    explicit StoneTile(const sf::Texture& texture) {
        setTexture(texture);
    }
};

// Derived class representing a grass tile
class GrassTile : public Tile {
public:
    explicit GrassTile(const sf::Texture& texture) {
        setTexture(texture);
    }
};

// Derived class representing a flower tile
class FlowerTile : public Tile {
public:
    explicit FlowerTile(const sf::Texture& texture) {
        setTexture(texture);
    }
};


// Derived class representing a water tile
class WaterTile : public Tile {
public:
    explicit WaterTile(const sf::Texture& texture) {
        setTexture(texture);
    }
};



#endif 
