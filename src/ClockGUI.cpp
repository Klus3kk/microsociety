#include "ClockGUI.hpp"
#include <cmath>
#include <stdexcept>
#include <sstream>

ClockGUI::ClockGUI(float x, float y) {
    if (!clockFont.loadFromFile("../assets/fonts/font.ttf")) {
        throw std::runtime_error("Failed to load clock font!");
    }

    // Initialize components
    setupClockFace();
    setupHands();

    // Position the clock
    clockCircle.setPosition(x, y);
    for (auto& number : clockNumbers) {
        number.move(x, y); // Adjust number positions relative to the clock
    }
    hourHand.setPosition(x, y);
    minuteHand.setPosition(x, y);
    secondHand.setPosition(x, y);
}

void ClockGUI::setupClockFace() {
    // Main circle
    clockCircle.setRadius(50.0f);
    clockCircle.setFillColor(sf::Color(30, 30, 30)); // Modern dark gray
    clockCircle.setOutlineThickness(3.0f);
    clockCircle.setOutlineColor(sf::Color(60, 60, 60)); // Subtle lighter outline
    clockCircle.setOrigin(clockCircle.getRadius(), clockCircle.getRadius());

    // Numbers
    for (int i = 1; i <= 12; ++i) {
        sf::Text numberText;
        numberText.setFont(clockFont);
        numberText.setCharacterSize(14);
        numberText.setFillColor(sf::Color::White);
        numberText.setString(std::to_string(i));

        // Calculate position
        float angle = static_cast<float>(i) * 30.0f - 90.0f; // 30 degrees per hour, start at 12
        float radian = angle * 3.14159265358979f / 180.0f;
        float x = std::cos(radian) * 40.0f;
        float y = std::sin(radian) * 40.0f;

        numberText.setPosition(x - numberText.getLocalBounds().width / 2,
                               y - numberText.getLocalBounds().height / 2);
        clockNumbers.push_back(numberText);
    }
}

void ClockGUI::setupHands() {
    // Hour hand
    hourHand.setSize(sf::Vector2f(25.0f, 5.0f));
    hourHand.setFillColor(sf::Color(200, 200, 200)); // Slightly gray white
    hourHand.setOrigin(0, hourHand.getSize().y / 2);

    // Minute hand
    minuteHand.setSize(sf::Vector2f(35.0f, 3.0f));
    minuteHand.setFillColor(sf::Color::White);
    minuteHand.setOrigin(0, minuteHand.getSize().y / 2);

    // Second hand
    secondHand.setSize(sf::Vector2f(40.0f, 1.5f));
    secondHand.setFillColor(sf::Color(255, 80, 80)); // Softer red
    secondHand.setOrigin(0, secondHand.getSize().y / 2);
}

void ClockGUI::update(float currentTime) {
    // Calculate hours, minutes, and seconds
    int hours = static_cast<int>(currentTime) / 3600 % 12;
    int minutes = (static_cast<int>(currentTime) % 3600) / 60;
    int seconds = static_cast<int>(currentTime) % 60;

    // Calculate angles for the hands
    float hourAngle = hours * 30.0f + minutes * 0.5f - 90.0f;
    float minuteAngle = minutes * 6.0f - 90.0f;
    float secondAngle = seconds * 6.0f - 90.0f;

    // Rotate the hands
    hourHand.setRotation(hourAngle);
    minuteHand.setRotation(minuteAngle);
    secondHand.setRotation(secondAngle);
}

void ClockGUI::render(sf::RenderWindow& window) {
    // Draw clock components
    window.draw(clockCircle);
    for (const auto& number : clockNumbers) {
        window.draw(number);
    }
    window.draw(hourHand);
    window.draw(minuteHand);
    window.draw(secondHand);
}
