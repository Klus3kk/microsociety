#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics.hpp>

class Object {
protected:
    sf::Sprite sprite;
    sf::Texture texture; // make sure that texture is declared here, because if you assign tex to the sprite, without assigning it to variable sf::Texture, it won't work
public:
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ~Object() = default;

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
};

#endif
