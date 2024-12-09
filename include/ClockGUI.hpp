#ifndef CLOCK_GUI_HPP
#define CLOCK_GUI_HPP

#include <SFML/Graphics.hpp>

class ClockGUI {
private:
    sf::CircleShape clockCircle;       // The main clock circle
    sf::RectangleShape hourHand;      // Hour hand
    sf::RectangleShape minuteHand;    // Minute hand
    sf::RectangleShape secondHand;    // Second hand
    sf::Font clockFont;               // Font for numbers
    std::vector<sf::Text> clockNumbers; // Numbers on the clock face

    // Setup helpers
    void setupClockFace();  // Set up circle and numbers
    void setupHands();      // Set up clock hands

public:
    ClockGUI(float x, float y); // Constructor with position
    void update(float currentTime); // Update clock hands based on time
    void render(sf::RenderWindow& window, bool isVisible); // Render the clock
};

#endif
