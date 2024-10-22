#ifndef GRASSTILE_HPP
#define GRASSTILE_HPP

#include "Tile.hpp"

class GrassTile : public Tile {
public:
    GrassTile(const sf::Texture& texture) {
        setTexture(texture);  // Assign one of three grass textures here
    }
};

#endif // GRASSTILE_HPP
