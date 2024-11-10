#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>
#include "Object.hpp"
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

    virtual void draw(sf::RenderWindow &window) const {
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
        return object ? object->getSprite().getGlobalBounds() : sf::FloatRect();
    }
};

class StoneTile : public Tile {
public:
    StoneTile(const sf::Texture &texture) {
        this->setTexture(texture);  
    }
    virtual ~StoneTile() {}
};

class GrassTile : public Tile {
public:
    GrassTile(const sf::Texture& texture) {
        setTexture(texture);  
    }
};

class FlowerTile : public Tile {
public:
    FlowerTile(const sf::Texture &texture) {
        this->setTexture(texture);  
    }
    virtual ~FlowerTile() {}
};


#endif
