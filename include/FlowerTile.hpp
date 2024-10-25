#ifndef FLOWERTILE_HPP
#define FLOWERTILE_HPP

#include "Tile.hpp"

class FlowerTile : public Tile {
public:
    FlowerTile(const sf::Texture &texture) {
        this->setTexture(texture);  
    }
    virtual ~FlowerTile() {}
};

#endif 
