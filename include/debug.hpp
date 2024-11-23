#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <chrono>

class Game; 
class PlayerEntity;

// Log severity levels
enum class LogLevel {
    Info,
    Warning,
    Error
};

// Debug system for in-game console
class DebugConsole {
private:
    std::vector<std::pair<std::string, std::string>> logs; // Logs with categories
    sf::Font consoleFont;                                  // Font for console
    sf::RectangleShape background;                         // Background for console UI
    sf::Text text;                                         // Text object for rendering logs
    const int maxLogs = 50;                                // Maximum number of logs shown
    const sf::Color backgroundColor = sf::Color(0, 0, 0, 150); // Opaque black background
    bool enabled = false;                                  // Toggle for enabling/disabling debug
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> throttleTimers;
    std::unordered_map<std::string, bool> logOnceTracker;  // Tracker for `logOnce`

public:
    DebugConsole(float windowWidth, float windowHeight);
    void toggle();
    void enable();
    void disable();
    bool isEnabled() const;

    // Log with category and severity level
    void log(const std::string& category, const std::string& message, LogLevel level = LogLevel::Info);
    // Throttled logging
    void logThrottled(const std::string& category, const std::string& message, int throttleMs);
    // Log once per session
    void logOnce(const std::string& category, const std::string& message);

    void render(sf::RenderWindow& window);
    void clearLogs(); // Clear all logs
};

// Singleton
DebugConsole& getDebugConsole();

// Debug helper functions
void debugTileInfo(int tileX, int tileY, const Game& game);
void debugMarketPrices(const std::unordered_map<std::string, float>& marketPrices);
void debugCollisionEvent(const std::string& message, int throttleMs = 500);
void debugActionPerformed(const std::string& actionName, const std::string& objectType);
void debugPlayerSpeed(float speed);

#endif
