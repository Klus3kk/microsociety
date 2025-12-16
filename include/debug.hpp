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

// Forward declarations to reduce dependencies
class Game;
class NPCEntity;

// Enum defining log severity levels
enum class LogLevel {
    Info,       // General information
    Warning,    // Potential issues
    Error,      // Errors that need attention
    Critical    // Severe errors requiring immediate action
};

// Debug system for in-game console
class DebugConsole {
private:
    std::vector<std::pair<std::string, std::string>> logs; // Stores logs with categories
    sf::Font consoleFont;  // Font used for rendering debug text
    sf::RectangleShape background; // UI background for the debug console
    sf::Text text;  // SFML text object to display log messages
    std::mutex debugMutex; // Ensures thread safety for logging

    const int maxLogs = 15; // Maximum number of logs stored at a time
    const sf::Color backgroundColor = sf::Color(0, 0, 0, 200); // Semi-transparent UI background
    bool enabled = false; // Flag to toggle debug console visibility

    LogLevel filterLevel = LogLevel::Info; // Current logging level filter
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> throttleTimers; // Stores timestamps for throttled logs
    std::unordered_map<std::string, bool> logOnceTracker; // Tracks messages that should be logged only once

    void trimLogs(); // Removes old logs when reaching maxLogs limit
    std::string getLogFilename() const; // Generates a timestamped filename for log storage

public:
    // Constructor initializes the debug console UI
    DebugConsole(float windowWidth, float windowHeight);

    // Toggle Debug Console visibility
    void toggle();  // Switch between enabled/disabled states
    void enable();  // Turn on debug console
    void disable(); // Turn off debug console
    bool isEnabled() const; // Check if debug console is active

    // Logging Methods
    void setLogLevel(LogLevel level); // Set the minimum log level for filtering
    void log(const std::string& category, const std::string& message, LogLevel level = LogLevel::Info); // Log a message with a category
    void logThrottled(const std::string& category, const std::string& message, int throttleMs); // Log a message but prevent spam by setting a time threshold
    void logOnce(const std::string& category, const std::string& message); // Log a message only once to prevent duplicates

    // System Logs
    void logSystemStats(float fps, size_t memoryUsage); // Logs FPS and memory usage stats
    void logResourceStats(const std::unordered_map<std::string, int>& resources); // Logs collected resources
    void saveLogToFile(const std::string& filename, const std::string& logEntry); // Save a single log entry to a file
    void saveLogsToFile(const std::string& filename); // Save current logs to a file
    void saveAllLogs(const std::string& filename); // Save all historical logs to a file

    // Rendering function for drawing the console onto the screen
    void render(sf::RenderWindow& window);

    // Clears all stored logs
    void clearLogs();
};

// Singleton pattern for accessing the DebugConsole globally
DebugConsole& getDebugConsole();

// Debug helper functions for various in-game events
void debugTileInfo(int tileX, int tileY, const Game& game); // Logs tile information
void debugMarketPrices(const std::unordered_map<std::string, float>& marketPrices); // Logs market price changes
void debugCollisionEvent(const std::string& message, int throttleMs = 500); // Logs collision detection messages
void debugActionPerformed(const std::string& actionName, const std::string& objectType); // Logs NPC actions
void debugNPCStats(const std::string& npcName, float health, float energy, float money); // Logs NPC attributes
void debugSimulationIteration(int iteration, float elapsedTime); // Logs simulation progress
void debugMarketTransactions(const std::unordered_map<std::string, int>& transactions); // Logs market trading details

#endif
