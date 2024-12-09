#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>
#include "Object.hpp"
#include <memory>

class Tile {
protected:
    sf::Sprite sprite;
    sf::Texture texture;
    std::unique_ptr<Object> object;

public:
    Tile() = default;
    virtual ~Tile() = default;

    virtual void setTexture(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
        if (object) {
            object->setPosition(x, y);
        }
    }

    virtual void draw(sf::RenderWindow& window) const {
        window.draw(sprite);
        if (object) {
            object->draw(window);
        }
    }

    void placeObject(std::unique_ptr<Object> obj) {
        object = std::move(obj);
        if (object) {
            object->setPosition(sprite.getPosition().x, sprite.getPosition().y);
        }
    }

    Object* getObject() const {
        return object.get();
    }

    bool hasObject() const {
        return object != nullptr;
    }

    sf::FloatRect getObjectBounds() const {
        return object ? object->getObjectBounds() : sf::FloatRect();
    }

    void removeObject() {
        object.reset();
    }
};

class StoneTile : public Tile {
public:
    explicit StoneTile(const sf::Texture& texture) {
        setTexture(texture);
    }
};

class GrassTile : public Tile {
public:
    explicit GrassTile(const sf::Texture& texture) {
        setTexture(texture);
    }
};

class FlowerTile : public Tile {
public:
    explicit FlowerTile(const sf::Texture& texture) {
        setTexture(texture);
    }
};

#endif
