#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>
#include "Object.hpp"
class Tile {
protected:
    sf::Sprite sprite;
    sf::Texture texture;
    std::unique_ptr<Object> object;

public:
    Tile() = default;
    virtual ~Tile() = default;

    virtual void setTexture(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
        if (object) {  // if statement for ensuring object position matches tile position
            object->setPosition(x, y); 
        }
    }

    virtual void draw(sf::RenderWindow &window) const {
        window.draw(sprite);
        if (object) { // if object exists, draw the object
            object->draw(window);  
        }
    }

    void placeObject(std::unique_ptr<Object> obj) {
        object = std::move(obj);
        if (object) {
            object->setPosition(sprite.getPosition().x, sprite.getPosition().y);
        }
    }

    Object* getObject() const {
        return object.get();  // return the stored object
    }

    bool hasObject() const {
        return object != nullptr;
    }

    sf::FloatRect getObjectBounds() const {
        return object ? object->getSprite().getGlobalBounds() : sf::FloatRect();
    }


};

#endif 



