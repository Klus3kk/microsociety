#include "UIButton.hpp"

UIButton::UIButton() {
    // Default colors
    normalColor = sf::Color(70, 70, 70, 255);
    hoverColor = sf::Color(100, 100, 100, 255);
    buttonShape.setFillColor(normalColor);
}

UIButton::UIButton(float x, float y, float width, float height, const std::string& text, const sf::Font& font) {
    setProperties(x, y, width, height, text, font);
    normalColor = sf::Color(70, 70, 70, 255);
    hoverColor = sf::Color(100, 100, 100, 255);
    buttonShape.setFillColor(normalColor);
}

void UIButton::setProperties(float x, float y, float width, float height, const std::string& text, const sf::Font& font) {
    // Configure button shape
    buttonShape.setPosition(x, y);
    buttonShape.setSize({width, height});
    buttonShape.setFillColor(normalColor);
    buttonShape.setOutlineThickness(2);
    buttonShape.setOutlineColor(sf::Color::White);

    // Configure button text
    buttonText.setString(text);
    buttonText.setFont(font);
    buttonText.setCharacterSize(16);
    buttonText.setFillColor(sf::Color::White);

    // Center text within button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        x + (width - textBounds.width) / 2 - textBounds.left,
        y + (height - textBounds.height) / 2 - textBounds.top
    );
}

void UIButton::setColors(const sf::Color& normal, const sf::Color& hover) {
    normalColor = normal;
    hoverColor = hover;
    buttonShape.setFillColor(normalColor);
}

void UIButton::handleHover(sf::RenderWindow& window) {
    // Change button color on hover
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (buttonShape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
        buttonShape.setFillColor(hoverColor);
    } else {
        buttonShape.setFillColor(normalColor);
    }
}

bool UIButton::isClicked(sf::RenderWindow& window, sf::Event& event) {
    // Check for mouse click within button bounds
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (buttonShape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
            return true;
        }
    }
    return false;
}

void UIButton::draw(sf::RenderWindow& window) {
    // Render button and text
    window.draw(buttonShape);
    window.draw(buttonText);
}
