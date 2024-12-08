#include "UIButton.hpp"
#include <iostream>

UIButton::UIButton()
    : isHovered(false), isClickedState(false), cornerRadius(8.0f), borderThickness(2.0f) {
    // Set default colors with transparency (80%)
    normalColor = sf::Color(50, 50, 50, 204);
    hoverColor = sf::Color(70, 70, 100, 204);
    clickColor = sf::Color(100, 100, 150, 204);
    borderColor = sf::Color(200, 200, 200, 204);

    buttonShape.setFillColor(normalColor);
    buttonShape.setOutlineThickness(borderThickness);
    buttonShape.setOutlineColor(borderColor);
}

UIButton::UIButton(float x, float y, float width, float height, const std::string& text, const sf::Font& font)
    : UIButton() { // Call default constructor
    setProperties(x, y, width, height, text, font);
}

void UIButton::setProperties(float x, float y, float width, float height, const std::string& text, const sf::Font& font) {
    buttonShape.setPosition(x, y);
    buttonShape.setSize({width, height});
    buttonShape.setFillColor(normalColor);
    buttonShape.setOutlineThickness(borderThickness);
    buttonShape.setOutlineColor(borderColor);

    buttonText.setString(text);
    buttonText.setFont(font);
    buttonText.setCharacterSize(18);
    buttonText.setFillColor(sf::Color::White);

    // Center text in the button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        x + (width - textBounds.width) / 2 - textBounds.left,
        y + (height - textBounds.height) / 2 - textBounds.top
    );
}

void UIButton::setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& click, const sf::Color& border) {
    normalColor = normal;
    hoverColor = hover;
    clickColor = click;
    borderColor = border;
    buttonShape.setOutlineColor(borderColor);
    buttonShape.setFillColor(normalColor); // Default to normal color
}

void UIButton::setStyle(float radius, float thickness) {
    cornerRadius = radius;
    borderThickness = thickness;
    buttonShape.setOutlineThickness(borderThickness);
}

void UIButton::handleHover(sf::RenderWindow& window) {
    isHovered = isMouseOver(window);

    if (isHovered) {
        buttonShape.setFillColor(hoverColor);
    } else {
        buttonShape.setFillColor(normalColor);
    }
}


void UIButton::handleClickAnimation() {
    if (isClickedState) {
        float elapsedTime = clickAnimationClock.getElapsedTime().asSeconds();
        if (elapsedTime > 0.15f) { // Reset after 0.15 seconds
            isClickedState = false;
            buttonShape.setFillColor(isHovered ? hoverColor : normalColor);
        }
    }
}


bool UIButton::isClicked(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left && isMouseOver(window)) {
        isClickedState = true;
        buttonShape.setFillColor(clickColor);
        return true;
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        isClickedState = false;
        buttonShape.setFillColor(isHovered ? hoverColor : normalColor);
    }
    return false;
}



bool UIButton::isMouseOver(sf::RenderWindow& window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePos); 
    return buttonShape.getGlobalBounds().contains(mouseWorldPos);
}

void UIButton::draw(sf::RenderWindow& window) {
    // Create a gradient effect by overlaying a semi-transparent rectangle
    sf::RectangleShape gradientShape(buttonShape.getSize());
    gradientShape.setPosition(buttonShape.getPosition());
    gradientShape.setFillColor(sf::Color(255, 255, 255, 50)); // Light gradient overlay

    // Draw the button base and gradient
    window.draw(buttonShape);
    window.draw(gradientShape);

    // Draw the button text
    window.draw(buttonText);
}

sf::Vector2f UIButton::getPosition() const {
    return buttonShape.getPosition();
}

sf::Vector2f UIButton::getSize() const {
    return buttonShape.getSize();
}

std::string UIButton::getText() const {
    return buttonText.getString();
}

void UIButton::setRelativePosition(const sf::Vector2f& position) {
    relativePosition = position;
}

sf::Vector2f UIButton::getRelativePosition() const {
    return relativePosition;
}
