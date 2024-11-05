#include "Tile.hpp"
#include "Entity.hpp"  // Include Entity only here

bool Tile::checkCollision(const Entity& entity) const {
    if (object) {
        return entity.getSprite().getGlobalBounds().intersects(object->getSprite().getGlobalBounds());
    }
    return false;
}
