#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>

class UIButton {
private:
    sf::RectangleShape buttonShape; // The base shape of the button
    sf::Text buttonText;            // Text displayed on the button
    sf::Font buttonFont;            // Font used for the text

    // Colors for different button states
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color clickColor;
    sf::Color borderColor;

    // Button properties
    float cornerRadius;
    float borderThickness;

    bool isHovered = false;
    bool isClickedState = false;

    // Animation and timing helpers
    sf::Clock clickAnimationClock;

    // Helpers for advanced appearance
    void applyGradient();
    void applyGlow();

public:
    UIButton();
    UIButton(float x, float y, float width, float height, const std::string& text, const sf::Font& font);

    void setProperties(float x, float y, float width, float height, const std::string& text, const sf::Font& font);
    void setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& click, const sf::Color& border);
    void setStyle(float cornerRadius, float borderThickness);

    void handleHover(sf::RenderWindow& window);
    void handleClickAnimation();
    bool isClicked(sf::RenderWindow& window, sf::Event& event);
    bool isMouseOver(sf::RenderWindow& window) const;

    void draw(sf::RenderWindow& window);
};

#endif
