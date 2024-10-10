#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <SFML/Graphics.hpp>

class Entity {
protected:
    int id;
    std::string name;
    sf::Vector2f position;

public:
    Entity(int id, const std::string& name, sf::Vector2f pos)  
    virtual ~Entity(); // Can be overwritten

    sf::Vector2f getPosition() const;  
}

#endif