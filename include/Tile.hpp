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
    }

    virtual void draw(sf::RenderWindow &window) const {
        window.draw(sprite);
        if (object) {
            object->draw(window);  // Draw the object
        }
    }

    void placeObject(std::unique_ptr<Object> obj) {
        object = std::move(obj);
        if (object) {
            object->setPosition(sprite.getPosition().x, sprite.getPosition().y);
        }
    }

};

#endif // TILE_HPP



