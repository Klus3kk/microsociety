#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <mutex>

// Forward declarations
class Game;
class NPCEntity;

// Log severity levels
enum class LogLevel {
    Info,
    Warning,
    Error,
    Critical 
};

// Debug system for in-game console
class DebugConsole {
private:
    std::vector<std::pair<std::string, std::string>> logs; // Logs with categories
    sf::Font consoleFont;                                  // Font for console
    sf::RectangleShape background;                         // Background for console UI
    sf::Text text;                                         // Text object for rendering logs
    std::mutex debugMutex;
    const int maxLogs = 15;                                // Increased number of logs shown
    const sf::Color backgroundColor = sf::Color(0, 0, 0, 200); // Semi-transparent background
    bool enabled = false;                                  // Toggle for enabling/disabling debug
    LogLevel filterLevel = LogLevel::Info;                 // Filter level for logs
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> throttleTimers;
    std::unordered_map<std::string, bool> logOnceTracker;  // Tracker for `logOnce`

    void trimLogs();                                       // Remove excess logs
    std::string formatLog(const std::string& category, const std::string& message, LogLevel level) const;

public:
    DebugConsole(float windowWidth, float windowHeight);

    // Enable/Disable Console
    void toggle();
    void enable();
    void disable();
    bool isEnabled() const;

    // Logging Methods
    void setLogLevel(LogLevel level);
    void log(const std::string& category, const std::string& message, LogLevel level = LogLevel::Info);
    void logThrottled(const std::string& category, const std::string& message, int throttleMs);
    void logOnce(const std::string& category, const std::string& message);

    // System Logs
    void logSystemStats(float fps, size_t memoryUsage); // Track FPS and memory
    void logResourceStats(const std::unordered_map<std::string, int>& resources); // For resource tracking
    void saveLogsToFile(const std::string& filename) const; // Save to file

    // Rendering
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
void debugNPCStats(const std::string& npcName, float health, float energy, float money); // Added for NPC tracking
void debugSimulationIteration(int iteration, float elapsedTime); // Added for iteration tracking
void debugMarketTransactions(const std::unordered_map<std::string, int>& transactions); // Added for detailed market logs

#endif // DEBUG_HPP
