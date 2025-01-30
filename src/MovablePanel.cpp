#include "MovablePanel.hpp"
#include "debug.hpp"

// Constructor: Initializes the panel with a given width, height, and title
MovablePanel::MovablePanel(float width, float height, const std::string& title)
    : panelShape(sf::Vector2f(width, height))  // Set panel size safely
    , isDragging(false)  // Dragging state initially false
    , dragOffset(0.0f, 0.0f) {  // Default drag offset

    panelShape.setPosition(0.0f, 0.0f);  // Default starting position at (0,0)
    panelShape.setFillColor(sf::Color(30, 30, 30, 200));  // Semi-transparent dark background
    panelShape.setOutlineThickness(2.0f);
    panelShape.setOutlineColor(sf::Color::White);  // White border for visibility

    // Load font safely
    if (!font.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    // Set title properties
    panelTitle.setFont(font);
    panelTitle.setCharacterSize(18);
    panelTitle.setString(title.empty() ? "" : title);  // Use title if provided
    panelTitle.setFillColor(sf::Color::White);  // White text color
    updateTextPosition();  // Ensure title is correctly positioned

    // Log an error if invalid dimensions are provided
    if (width <= 0 || height <= 0) {
        getDebugConsole().log("ERROR", "Invalid size for MovablePanel: " + std::to_string(width) + "x" + std::to_string(height));
    }
}

// Destructor: Clears child buttons safely
MovablePanel::~MovablePanel() {
    childButtons.clear();  // Remove all stored UI buttons
}

// Handles user input events for dragging the panel
void MovablePanel::handleEvent(const sf::RenderWindow& window, sf::Event& event) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);  // Get mouse position

    // Check if the panel is clicked to start dragging
    if (event.type == sf::Event::MouseButtonPressed &&
        panelShape.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        isDragging = true;
        dragOffset = panelShape.getPosition() - sf::Vector2f(mousePos.x, mousePos.y);
    }

    // Stop dragging when the mouse is released
    if (event.type == sf::Event::MouseButtonReleased) {
        isDragging = false;
    }

    // If dragging, update the panel position
    if (isDragging && event.type == sf::Event::MouseMoved) {
        panelShape.setPosition(static_cast<float>(mousePos.x) + dragOffset.x,
                               static_cast<float>(mousePos.y) + dragOffset.y);
        updateTextPosition();  // Update title position after moving
        updateChildPositions(); // Update positions of child UI elements
    }
}

// Updates the size of the panel while keeping the minimum size constraint
void MovablePanel::setSize(float width, float height) {
    panelShape.setSize(sf::Vector2f(std::max(10.0f, width), std::max(10.0f, height)));
    updateTextPosition();  // Adjust title position
    updateChildPositions(); // Update child buttons
}

// Updates the panel title and repositions it accordingly
void MovablePanel::setTitle(const std::string& title) {
    panelTitle.setString(title);
    updateTextPosition(); // Adjust title placement dynamically
}

// Sets the position of the panel and updates the position of child elements
void MovablePanel::setPosition(float x, float y) {
    panelShape.setPosition(x, y);
    updateTextPosition();  // Adjust title position
    updateChildPositions(); // Adjust child button positions
}

// Adds a new button as a child element of the panel
void MovablePanel::addChild(UIButton* button) {
    if (!button) return;  // Ignore null pointers

    // Prevent duplicate buttons from being added
    if (std::find(childButtons.begin(), childButtons.end(), button) != childButtons.end()) {
        return;  // Button is already added
    }

    // Store the relative position of the button in relation to the panel
    sf::Vector2f panelPosition = panelShape.getPosition();
    sf::Vector2f buttonPosition = button->getPosition();
    sf::Vector2f relativeOffset = buttonPosition - panelPosition;

    button->setRelativePosition(relativeOffset);
    childButtons.push_back(button);  // Store button in child list
    updateChildPositions();  // Reposition buttons
}

// Removes all child buttons and ensures memory is safely released
void MovablePanel::clearChildren() {
    for (auto& button : childButtons) {
        if (button) {
            delete button;   // Free dynamically allocated memory
            button = nullptr;  // Avoid dangling pointer
        }
    }
    childButtons.clear();  // Empty the child list
}

// Renders the panel and all its child UI elements
void MovablePanel::render(sf::RenderWindow& window) {
    window.draw(panelShape);  // Draw panel background
    window.draw(panelTitle);  // Draw panel title
    for (UIButton* button : childButtons) {
        if (button) {
            button->draw(window);  // Draw all buttons inside the panel
        }
    }
}

// Returns the bounds of the panel (for collision or UI interaction)
sf::FloatRect MovablePanel::getBounds() const {
    return panelShape.getGlobalBounds();
}

// Adjusts the position of the title text relative to the panel
void MovablePanel::updateTextPosition() {
    panelTitle.setPosition(
        panelShape.getPosition().x + (panelShape.getSize().x - panelTitle.getLocalBounds().width) / 2,
        panelShape.getPosition().y + 10  // Add a small padding from the top
    );
}

// Repositions all child buttons inside the panel
void MovablePanel::updateChildPositions() {
    sf::Vector2f panelPosition = panelShape.getPosition();
    float startY = panelTitle.getPosition().y + panelTitle.getCharacterSize() + 20; // Space below the title
    float buttonSpacing = 10.0f;

    // Position each button inside the panel
    for (UIButton* button : childButtons) {
        button->setProperties(
            panelPosition.x + (panelShape.getSize().x - button->getSize().x) / 2, // Center horizontally
            startY, // Vertical stacking
            button->getSize().x, // Maintain button width
            button->getSize().y, // Maintain button height
            button->getText(),   // Preserve text
            font                 // Preserve font
        );
        startY += button->getSize().y + buttonSpacing; // Move to next button position
    }
}
