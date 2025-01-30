#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>

// UIButton class encapsulates button behavior in a UI
class UIButton {
private:
    sf::RectangleShape buttonShape; // The base shape of the button
    sf::Text buttonText;            // Text displayed on the button
    sf::Font buttonFont;            // Font used for the text

    // Colors for different button states
    sf::Color normalColor; // Default button color
    sf::Color hoverColor;  // Color when hovered
    sf::Color clickColor;  // Color when clicked
    sf::Color borderColor; // Border color

    // Button properties
    float cornerRadius;    // Rounded corners (future feature)
    float borderThickness; // Border thickness

    bool isHovered;       // Tracks if the button is hovered
    bool isClickedState;  // Tracks if the button is clicked

    // Animation and timing helpers
    sf::Clock clickAnimationClock; // Handles click animation timing

    sf::Vector2f relativePosition; // Relative position for UI scaling

public:
    // Default constructor
    UIButton();

    // Parameterized constructor (sets button properties immediately)
    UIButton(float x, float y, float width, float height, const std::string& text, const sf::Font& font);

    // Set position, size, text, and font of the button
    void setProperties(float x, float y, float width, float height, const std::string& text, const sf::Font& font);

    // Define button colors for different states
    void setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& click, const sf::Color& border);

    // Set corner radius and border thickness
    void setStyle(float cornerRadius, float borderThickness);

    // Handle hover effects (change color when hovered)
    void handleHover(sf::RenderWindow& window);

    // Handle click animation (simulate click effect)
    void handleClickAnimation();

    // Detect if the button is clicked
    bool isClicked(sf::RenderWindow& window, sf::Event& event);

    // Check if the mouse is over the button
    bool isMouseOver(sf::RenderWindow& window) const;

    // Set relative position (useful for UI scaling)
    void setRelativePosition(const sf::Vector2f& position);

    // Render the button on the window
    void draw(sf::RenderWindow& window);

    // Get button position
    sf::Vector2f getPosition() const;

    // Get button size
    sf::Vector2f getSize() const;

    // Get button text content
    std::string getText() const;

    // Get relative position
    sf::Vector2f getRelativePosition() const;
}; 

#endif
