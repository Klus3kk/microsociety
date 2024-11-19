#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>

class UIButton {
private:
    sf::RectangleShape buttonShape;  // The shape of the button
    sf::Text buttonText;             // The text displayed on the button
    sf::Font buttonFont;             // Font for the text
    sf::Color normalColor;           // Default button color
    sf::Color hoverColor;            // Color when hovered

public:
    UIButton();
    UIButton(float x, float y, float width, float height, const std::string& text, const sf::Font& font);

    void setProperties(float x, float y, float width, float height, const std::string& text, const sf::Font& font);
    void setColors(const sf::Color& normal, const sf::Color& hover);

    void handleHover(sf::RenderWindow& window);
    bool isClicked(sf::RenderWindow& window, sf::Event& event);

    void draw(sf::RenderWindow& window);
};

#endif // UIBUTTON_HPP
