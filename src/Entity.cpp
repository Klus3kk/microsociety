#include "Entity.hpp"

Entity::Entity(int id, const std::string& name, sf::Vector2f pos, sf::Texture texture, sf::Sprite sprite(texture)): 
    id(id), name(name), position(pos), texture(texture), sprite(sprite) {}

Entity::~Entity() {}

sf::Vector2f Entity::getPosition() const {
    return position;
}