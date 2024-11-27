#include "MovablePanel.hpp"

MovablePanel::MovablePanel(float width, float height, const std::string& title) {
    panelShape.setSize({width, height});
    panelShape.setFillColor(sf::Color(30, 30, 30, 200)); // Dark, semi-transparent
    panelShape.setOutlineThickness(2.0f);
    panelShape.setOutlineColor(sf::Color::White);

    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    panelTitle.setFont(font);
    panelTitle.setCharacterSize(18);
    panelTitle.setString(title);
    panelTitle.setFillColor(sf::Color::White);
    panelTitle.setPosition(10, 10); // Title position
}

void MovablePanel::handleEvent(const sf::RenderWindow& window, sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    if (event.type == sf::Event::MouseButtonPressed &&
        panelShape.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        isDragging = true;
        dragOffset = panelShape.getPosition() - sf::Vector2f(mousePos.x, mousePos.y);
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        isDragging = false;
    }
    if (isDragging && event.type == sf::Event::MouseMoved) {
        panelShape.setPosition(static_cast<float>(mousePos.x) + dragOffset.x,
                               static_cast<float>(mousePos.y) + dragOffset.y);
    }
}

void MovablePanel::setSize(float width, float height) {
    panelShape.setSize({width, height});
}

void MovablePanel::setTitle(const std::string& title) {
    panelTitle.setString(title);
}

void MovablePanel::setPosition(float x, float y) {
    panelShape.setPosition(x, y);
}

void MovablePanel::render(sf::RenderWindow& window) {
    window.draw(panelShape);
    window.draw(panelTitle);
}

sf::FloatRect MovablePanel::getBounds() const {
    return panelShape.getGlobalBounds();
}
