#include "UIButton.hpp"
#include <iostream>

// Default constructor initializes button properties
UIButton::UIButton()
    : isHovered(false), isClickedState(false), cornerRadius(8.0f), borderThickness(2.0f) {
    // Set default colors with transparency (80%)
    normalColor = sf::Color(50, 50, 50, 204);    // Default color
    hoverColor = sf::Color(70, 70, 100, 204);    // Color when hovered
    clickColor = sf::Color(100, 100, 150, 204);  // Color when clicked
    borderColor = sf::Color(200, 200, 200, 204); // Border color

    buttonShape.setFillColor(normalColor);
    buttonShape.setOutlineThickness(borderThickness);
    buttonShape.setOutlineColor(borderColor);
}

// Constructor with position, size, text, and font
UIButton::UIButton(float x, float y, float width, float height, const std::string& text, const sf::Font& font)
    : UIButton() { // Calls default constructor to initialize default values
    setProperties(x, y, width, height, text, font);
}

// Sets button properties: position, size, text, and font
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

    // Center text inside the button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        x + (width - textBounds.width) / 2 - textBounds.left,
        y + (height - textBounds.height) / 2 - textBounds.top
    );
}

// Sets colors for normal, hover, click, and border states
void UIButton::setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& click, const sf::Color& border) {
    normalColor = sf::Color(80, 80, 80, 180);  // Darker transparent
    hoverColor = sf::Color(100, 100, 120, 220);
    clickColor = sf::Color(140, 140, 180, 255);
    borderColor = sf::Color(200, 200, 200, 255);
    
    buttonShape.setOutlineColor(borderColor);
    buttonShape.setFillColor(normalColor);
}

// Sets button corner radius and border thickness
void UIButton::setStyle(float radius, float thickness) {
    cornerRadius = radius;
    borderThickness = thickness;
    buttonShape.setOutlineThickness(borderThickness);
}

// Handles hover logic: changes button color when hovered
void UIButton::handleHover(sf::RenderWindow& window) {
    isHovered = isMouseOver(window);

    if (isHovered) {
        buttonShape.setFillColor(hoverColor);
    } else {
        buttonShape.setFillColor(normalColor);
    }
}

// Handles click animation: keeps the button "pressed" for a short time
void UIButton::handleClickAnimation() {
    if (isClickedState) {
        float elapsedTime = clickAnimationClock.getElapsedTime().asSeconds();
        if (elapsedTime > 0.15f) { // Reset after 0.15 seconds
            isClickedState = false;
            buttonShape.setFillColor(isHovered ? hoverColor : normalColor);
        }
    }
}

// Handles mouse click events: changes button color and updates state
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

// Checks if the mouse is over the button
bool UIButton::isMouseOver(sf::RenderWindow& window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePos); 
    return buttonShape.getGlobalBounds().contains(mouseWorldPos);
}

// Draws the button with a gradient effect
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

// Returns button position
sf::Vector2f UIButton::getPosition() const {
    return buttonShape.getPosition();
}

// Returns button size
sf::Vector2f UIButton::getSize() const {
    return buttonShape.getSize();
}

// Returns button text content
std::string UIButton::getText() const {
    return buttonText.getString();
}

// Sets a relative position (useful for dynamic UI positioning)
void UIButton::setRelativePosition(const sf::Vector2f& position) {
    relativePosition = position;
}

// Returns the relative position of the button
sf::Vector2f UIButton::getRelativePosition() const {
    return relativePosition;
}
