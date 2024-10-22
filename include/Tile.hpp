#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>

class Tile {
protected:
    sf::Sprite sprite;
    sf::Texture texture;

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
    }
};

#endif // TILE_HPP
