#ifndef CLOCK_GUI_HPP
#define CLOCK_GUI_HPP

#include <SFML/Graphics.hpp>

// heres the definition of the clock gui class
class ClockGUI {
private:
    sf::CircleShape clockCircle;        // The main clock circle
    sf::RectangleShape hourHand;        // Hour hand
    sf::RectangleShape minuteHand;      // Minute hand
    // sf::RectangleShape secondHand;   // Second hand
    sf::Font clockFont;                 // Font for numbers
    std::vector<sf::Text> clockNumbers; // Numbers on the clock face

    // setup helpers
    void setupClockFace();
    void setupHands();                                      

public:
    ClockGUI(float x, float y);

    // update clock hands based on time
    void update(float currentTime);                         
    void render(sf::RenderWindow& window, bool isVisible);  // Render the clock if it's visible
    void reset();                                           
};

#endif
