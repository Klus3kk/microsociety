#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics.hpp>

class Object {
protected:
    sf::Sprite sprite;
    sf::Texture texture;
public:
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ~Object() = default;

    void setPosition(float x, float y) {
        sprite.setPosition(x, y);  // Set the sprite position
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

#endif // OBJECT_HPP
