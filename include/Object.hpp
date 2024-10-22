#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics.hpp>

class Object {
protected:
    sf::Sprite sprite;
public:
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ~Object() = default;
    virtual void setPosition(float x, float y) {
        sprite.setPosition(x, y);  // Set the sprite position
    }
};

class Tree : public Object {
public:
    Tree(const sf::Texture& texture) {
        sprite.setTexture(texture);
    }
    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }
};

class Rock : public Object {
public:
    Rock(const sf::Texture& texture) {
        sprite.setTexture(texture);
    }
    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }
};

class Bush : public Object {
public:
    Bush(const sf::Texture& texture) {
        sprite.setTexture(texture);
    }
    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }
};

#endif // OBJECT_HPP
