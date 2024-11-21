#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <chrono>

class Game; // Forward declaration of Game class
class PlayerEntity;

// Debug system for centralized logging
class DebugConsole {
private:
    bool enabled = false;                          // Toggle for enabling/disabling debug
    std::vector<std::string> logs;                // Buffer for logs
    sf::Font consoleFont;                         // Font for console
    sf::RectangleShape background;                // Background for console UI
    sf::Text text;                                // Text object for rendering logs
    int maxLogs = 10;                             // Maximum number of logs shown
    sf::Color backgroundColor = sf::Color(0, 0, 0, 200); // Opaque black background

public:
    DebugConsole(float windowWidth, float windowHeight);
    void toggle();
    void enable();
    void disable();
    bool isEnabled() const;

    void log(const std::string& message);
    void render(sf::RenderWindow& window);

    void scrollUp();
    void scrollDown();
};

// Singleton instance
DebugConsole& getDebugConsole();

// Debug helper functions
void debugPlayerInfo(const PlayerEntity& player);
void debugTileInfo(int tileX, int tileY, const Game& game);
void debugMarketPrices(const std::unordered_map<std::string, float>& marketPrices);
void debugCollisionEvent(const std::string& message, int throttleMs = 500);
void debugActionPerformed(const std::string& actionName, const std::string& objectType);


#endif // DEBUG_HPP
