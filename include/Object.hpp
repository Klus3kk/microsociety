#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics.hpp>

enum class ObjectType {
    None,
    Tree,
    Rock,
    Bush,
    House,
    Market
};

class Object {
protected:
    sf::Sprite sprite;
    sf::Texture texture; // make sure that texture is declared here, because if you assign tex to the sprite, without assigning it to variable sf::Texture, it won't work
public:
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ~Object() = default;
    virtual ObjectType getType() const = 0; // Getting a type
    
    sf::Sprite& getSprite() {
        return sprite;
    }

    virtual sf::FloatRect getObjectBounds() const {
        // Expanded the object's bounds to make collision detection more forgiving
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bounds.left -= 2;
        bounds.top -= 2;
        bounds.width += 4;
        bounds.height += 4;
        return bounds;
    }

    void setPosition(float x, float y) {
        sprite.setPosition(x, y); 
    }

    void setSize(float scaleX, float scaleY) {
        sprite.setScale(scaleX, scaleY);  
    }
};

class Tree : public Object {
public:
    Tree(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    ObjectType getType() const override {
        return ObjectType::Tree;
    }
};

class Rock : public Object {
public:
    Rock(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    ObjectType getType() const override {
        return ObjectType::Rock;
    }
};


class Bush : public Object {
public:
    Bush(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    ObjectType getType() const override {
        return ObjectType::Bush;
    }
};


#endif
