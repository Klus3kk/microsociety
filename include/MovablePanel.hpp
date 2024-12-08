#ifndef MOVABLEPANEL_HPP
#define MOVABLEPANEL_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "UIButton.hpp"

class MovablePanel {
private:
    sf::RectangleShape panelShape;
    sf::Text panelTitle;
    sf::Font font;
    bool isDragging = false;
    sf::Vector2f dragOffset;

    std::vector<UIButton*> childButtons; // Store pointers to child buttons

    void updateTextPosition(); // Updates title text position relative to the panel
    void updateChildPositions(); // Updates child button positions relative to the panel

public:
    MovablePanel(float width, float height, const std::string& title);
    ~MovablePanel(); // Destructor to clean up child buttons

    // Event handling for dragging
    void handleEvent(const sf::RenderWindow& window, sf::Event& event);

    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setTitle(const std::string& title);

    void addChild(UIButton* button); // Add a child button
    void clearChildren();
    // Rendering
    void render(sf::RenderWindow& window);

    // Accessors
    sf::FloatRect getBounds() const;
};

#endif
