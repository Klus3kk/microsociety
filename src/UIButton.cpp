#include "UIButton.hpp"

UIButton::UIButton()
    : cornerRadius(8.0f), borderThickness(2.0f) {
    normalColor = sf::Color(50, 50, 50, 255);
    hoverColor = sf::Color(70, 70, 100, 255);
    clickColor = sf::Color(100, 100, 150, 255);
    borderColor = sf::Color::White;

    buttonShape.setFillColor(normalColor);
}

UIButton::UIButton(float x, float y, float width, float height, const std::string& text, const sf::Font& font)
    : cornerRadius(8.0f), borderThickness(2.0f) {
    setProperties(x, y, width, height, text, font);
    normalColor = sf::Color(50, 50, 50, 255);
    hoverColor = sf::Color(70, 70, 100, 255);
    clickColor = sf::Color(100, 100, 150, 255);
    borderColor = sf::Color::White;

    buttonShape.setFillColor(normalColor);
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
    buttonShape.setFillColor(normalColor);
}

void UIButton::setStyle(float radius, float thickness) {
    cornerRadius = radius;
    borderThickness = thickness;
    buttonShape.setOutlineThickness(borderThickness);
}

void UIButton::handleHover(sf::RenderWindow& window) {
    if (isMouseOver(window)) {
        isHovered = true;
        buttonShape.setFillColor(hoverColor);
        applyGlow();
    } else {
        isHovered = false;
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
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (isMouseOver(window)) {
            isClickedState = true;
            buttonShape.setFillColor(clickColor);
            clickAnimationClock.restart();
            return true;
        }
    }
    return false;
}

bool UIButton::isMouseOver(sf::RenderWindow& window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    return buttonShape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void UIButton::applyGradient() {
    // Use shaders or blend colors for a gradient effect (requires SFML shaders)
}

void UIButton::applyGlow() {
    buttonShape.setOutlineColor(sf::Color(200, 200, 255, 255)); // Light blue glow
}

void UIButton::draw(sf::RenderWindow& window) {
    handleClickAnimation();
    window.draw(buttonShape);
    window.draw(buttonText);
}
