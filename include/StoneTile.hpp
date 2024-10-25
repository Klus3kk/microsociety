#ifndef STONETILE_HPP
#define STONETILE_HPP

#include "Tile.hpp"

class StoneTile : public Tile {
public:
    StoneTile(const sf::Texture &texture) {
        this->setTexture(texture);  
    }
    virtual ~StoneTile() {}
};

#endif 
