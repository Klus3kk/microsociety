#include "MovablePanel.hpp"
#include "debug.hpp"
MovablePanel::MovablePanel(float width, float height, const std::string& title)
    : panelShape(sf::Vector2f(width, height))  // Set size safely
    , isDragging(false)
    , dragOffset(0.0f, 0.0f) {  // Default drag offset
    panelShape.setPosition(0.0f, 0.0f);  // Default position
    panelShape.setFillColor(sf::Color(30, 30, 30, 200));
    panelShape.setOutlineThickness(2.0f);
    panelShape.setOutlineColor(sf::Color::White);

    // Load font safely
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    // Set title safely
    panelTitle.setFont(font);
    panelTitle.setCharacterSize(18);
    panelTitle.setString(title.empty() ? "Panel" : title);
    panelTitle.setFillColor(sf::Color::White);
    updateTextPosition();

    if (width <= 0 || height <= 0) {
        getDebugConsole().log("ERROR", "Invalid size for MovablePanel: " + std::to_string(width) + "x" + std::to_string(height));
    }
}



MovablePanel::~MovablePanel() {
    // No need for explicit clearChildren here
    childButtons.clear();  // Safely clears the vector
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
        updateTextPosition();  // Update title position
        updateChildPositions(); // Update child positions
    }
}

void MovablePanel::setSize(float width, float height) {
    panelShape.setSize(sf::Vector2f(std::max(10.0f, width), std::max(10.0f, height)));
    updateTextPosition();
    updateChildPositions();
}



void MovablePanel::setTitle(const std::string& title) {
    panelTitle.setString(title);
    updateTextPosition(); // Adjust title placement if necessary
}

void MovablePanel::setPosition(float x, float y) {
    panelShape.setPosition(x, y);
    updateTextPosition();
    updateChildPositions();
}

void MovablePanel::addChild(UIButton* button) {
    if (!button) return;

    // Prevent adding duplicate pointers
    if (std::find(childButtons.begin(), childButtons.end(), button) != childButtons.end()) {
        return;  // Already added
    }

    sf::Vector2f panelPosition = panelShape.getPosition();
    sf::Vector2f buttonPosition = button->getPosition();
    sf::Vector2f relativeOffset = buttonPosition - panelPosition;

    button->setRelativePosition(relativeOffset);
    childButtons.push_back(button);
    updateChildPositions();
}


void MovablePanel::clearChildren() {
    for (auto& button : childButtons) {
        if (button) {
            delete button;   // Safely delete
            button = nullptr;  // Avoid dangling pointers
        }
    }
    childButtons.clear();  // Clear vector
}



void MovablePanel::render(sf::RenderWindow& window) {
    window.draw(panelShape);
    window.draw(panelTitle);
    for (UIButton* button : childButtons) {
        if (button) {
            button->draw(window);
        }
    }
}

sf::FloatRect MovablePanel::getBounds() const {
    return panelShape.getGlobalBounds();
}

void MovablePanel::updateTextPosition() {
    // Center title horizontally and leave space above for padding
    panelTitle.setPosition(
        panelShape.getPosition().x + (panelShape.getSize().x - panelTitle.getLocalBounds().width) / 2,
        panelShape.getPosition().y + 10
    );
}

void MovablePanel::updateChildPositions() {
    sf::Vector2f panelPosition = panelShape.getPosition();
    float startY = panelTitle.getPosition().y + panelTitle.getCharacterSize() + 20; // Space below the title
    float buttonSpacing = 10.0f;

    for (UIButton* button : childButtons) {
        button->setProperties(
            panelPosition.x + (panelShape.getSize().x - button->getSize().x) / 2, // Center horizontally
            startY, // Incremental Y position
            button->getSize().x, // Preserve width
            button->getSize().y, // Preserve height
            button->getText(),   // Preserve text
            font                 // Preserve font
        );
        startY += button->getSize().y + buttonSpacing; // Add spacing for the next button
    }
}
