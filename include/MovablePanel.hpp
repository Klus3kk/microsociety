#ifndef MOVABLEPANEL_HPP
#define MOVABLEPANEL_HPP

#include <SFML/Graphics.hpp>
#include <string>

class MovablePanel {
private:
    sf::RectangleShape panelShape;
    sf::Text panelTitle;
    sf::Font font;
    bool isDragging = false;
    sf::Vector2f dragOffset;

public:
    MovablePanel(float width, float height, const std::string& title);

    // Event handling for dragging
    void handleEvent(const sf::RenderWindow& window, sf::Event& event);
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setTitle(const std::string& title);

    // Rendering
    void render(sf::RenderWindow& window);

    // Accessors
    sf::FloatRect getBounds() const;
};

#endif
