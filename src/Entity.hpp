#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <SFML/Graphics.hpp>

class Entity {
protected:
    int id;
    std::string name;
    sf::Vector2f position;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Entity(int id, const std::string& name, sf::Vector2f pos, sf::Texture texture);  
    virtual ~Entity();

    sf::Vector2f getPosition() const;
};

#endif // ENTITY_HPP
