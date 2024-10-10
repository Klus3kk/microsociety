#include "Entity.hpp"

Entity::Entity(int id, const std::string& name, sf::Vector2f pos): id(id), name(name), position(pos) {}

Entity::~Entity() {}

sf::Vector2f Entity::getPosition() const {
    return position;
}