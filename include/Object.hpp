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
    sf::Texture texture;

public:
    virtual ~Object() = default;

    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ObjectType getType() const = 0;

    // Common functionalities
    sf::Sprite& getSprite() {
        return sprite;
    }

    virtual sf::FloatRect getObjectBounds() const {
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

    // New method to set texture in the base class
    void setTexture(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }
};

class Tree : public Object {
public:
    Tree(const sf::Texture& tex) {
        setTexture(tex);
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
        setTexture(tex);
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
        setTexture(tex);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    ObjectType getType() const override {
        return ObjectType::Bush;
    }
};

#endif // OBJECT_HPP
