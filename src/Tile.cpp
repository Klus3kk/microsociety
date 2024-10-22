#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>

class Tile {
protected:
    sf::Sprite sprite;

public:
    Tile(const sf::Texture &texture) {
        sprite.setTexture(texture);
    }
    virtual ~Tile() = default;

    virtual void draw(sf::RenderWindow &window) const {
        window.draw(sprite);
    }

    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
    }
};

#endif // TILE_HPP
