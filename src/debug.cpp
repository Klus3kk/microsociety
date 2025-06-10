#include "debug.hpp"
#include "Game.hpp"
#include "NPCEntity.hpp"
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <filesystem>

// Constructor for DebugConsole
DebugConsole::DebugConsole(float windowWidth, float windowHeight) {
    if (!consoleFont.loadFromFile("../assets/fonts/font.ttf")) {
        std::cerr << "Failed to load console font!" << std::endl; // Error handling
    }

    text.setFont(consoleFont);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::White);

    background.setSize({windowWidth, 200}); // Debug panel size
    background.setFillColor(backgroundColor); // Background transparency
    background.setPosition(0, windowHeight - 200); // Positioning the debug panel
}

// Toggle debug console visibility
void DebugConsole::toggle() { enabled = !enabled; }
void DebugConsole::enable() { enabled = true; }
void DebugConsole::disable() { enabled = false; }
bool DebugConsole::isEnabled() const { return enabled; }

// Set logging level to filter messages
void DebugConsole::setLogLevel(LogLevel level) {
    filterLevel = level;
}

// Save a single log entry to a file
void DebugConsole::saveLogToFile(const std::string& filename, const std::string& logEntry) {
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open()) return;
    outFile << logEntry << "\n";
}

// Save all logs to a file
void DebugConsole::saveAllLogs(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open()) return;

    for (const auto& [category, message] : logs) {
        outFile << message << "\n";
    }

    std::cout << "All logs saved to " << filename << std::endl;
}

// Generate a log filename based on current date
std::string DebugConsole::getLogFilename() const {
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&timeT);

    std::ostringstream filename;
    filename << "logs/" << std::put_time(&localTime, "%Y-%m-%d") << "_log.txt";
    return filename.str();
}

// Log a message with timestamp
void DebugConsole::log(const std::string& category, const std::string& message, LogLevel level) {
    if (level < filterLevel) return; // Filter logs based on level

    std::ostringstream formattedMessage;
    
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&timeT);

    formattedMessage << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] ";
    formattedMessage << "[" << category << "] " << message;

    {
        std::lock_guard<std::mutex> lock(debugMutex); // Thread safety
        logs.emplace_back(category, formattedMessage.str());
        trimLogs(); // Remove old logs if necessary
    }

    saveLogToFile(getLogFilename(), formattedMessage.str());
}

// Log a message with a throttle to prevent spam
void DebugConsole::logThrottled(const std::string& category, const std::string& message, int throttleMs) {
    auto now = std::chrono::high_resolution_clock::now();
    std::string key = category + ":" + message;
    auto& lastLogTime = throttleTimers[key];

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLogTime).count() > throttleMs) {
        log(category, message);
        lastLogTime = now;
    }
}

// Log a message only once
void DebugConsole::logOnce(const std::string& category, const std::string& message) {
    std::string key = category + ":" + message;
    if (!logOnceTracker[key]) {
        log(category, message);
        logOnceTracker[key] = true;
    }
}


// Log system stats such as FPS and memory usage
void DebugConsole::logSystemStats(float fps, size_t memoryUsage) {
    std::ostringstream oss;
    oss << "FPS: " << std::fixed << std::setprecision(2) << fps
        << ", Memory Usage: " << memoryUsage << " KB";
    log("System", oss.str(), LogLevel::Info);
}

// Log resource statistics for AI monitoring
void DebugConsole::logResourceStats(const std::unordered_map<std::string, int>& resources) {
    std::ostringstream oss;
    oss << "Resource Stats: ";
    for (const auto& [resource, quantity] : resources) {
        oss << resource << ": " << quantity << " ";
    }
    log("Resources", oss.str());
}

// Save logs to a file
void DebugConsole::saveLogsToFile(const std::string& filename) {
    std::filesystem::path logDir = "logs";
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directory(logDir);  // Create logs folder if missing
    }

    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open()) return;

    std::lock_guard<std::mutex> lock(debugMutex); // Ensure thread safety
    for (const auto& [category, message] : logs) {
        outFile << message << "\n";
    }

    std::cout << "Logs saved to " << filename << std::endl;
}

// Render the debug console in the game window
void DebugConsole::render(sf::RenderWindow& window) {
    if (!enabled) return;

    window.draw(background);

    float yOffset = background.getPosition().y + 10;
    size_t start = logs.size() > maxLogs ? logs.size() - maxLogs : 0;
    for (size_t i = start; i < logs.size(); ++i) {
        const auto& [category, message] = logs[i];
        text.setString(message);
        text.setPosition(10, yOffset);
        window.draw(text);
        yOffset += 16;
    }
}

// Clear all logs
void DebugConsole::clearLogs() {
    logs.clear();
}

// Trim logs to maintain performance
void DebugConsole::trimLogs() {
    if (logs.size() > 1000) logs.erase(logs.begin(), logs.begin() + (logs.size() - 1000));
}

// Singleton instance for DebugConsole
DebugConsole& getDebugConsole() {
    static DebugConsole instance(800, 800); // Adjust size based on game window
    return instance;
}

// Helper function: Log tile information
void debugTileInfo(int tileX, int tileY, const Game& game) {
    static auto lastTileLog = std::make_pair(-1, -1);
    if (lastTileLog != std::make_pair(tileX, tileY)) {
        std::ostringstream oss;
        oss << "Tile (" << tileX << ", " << tileY << ") ";
        auto& tile = game.getTileMap()[tileY][tileX];
        if (tile->hasObject()) {
            oss << "contains object.";
        } else {
            oss << "is empty.";
        }
        getDebugConsole().log("Tile", oss.str());
        lastTileLog = {tileX, tileY};
    }
}

// Log market price changes
void debugMarketPrices(const std::unordered_map<std::string, float>& marketPrices) {
    std::ostringstream oss;
    for (const auto& [resource, price] : marketPrices) {
        oss << resource << ": $" << price << " ";
    }
    getDebugConsole().log("Market", oss.str());
}

// Log collision events with throttling
void debugCollisionEvent(const std::string& message, int throttleMs) {
    getDebugConsole().logThrottled("Collision", message, throttleMs);
}

// Log actions performed by NPCs
void debugActionPerformed(const std::string& actionName, const std::string& objectType) {
    getDebugConsole().log("Action", actionName + " on " + objectType);
}

// Log NPC statistics
void debugNPCStats(const std::string& npcName, float health, float energy, float money) {
    std::ostringstream oss;
    oss << "NPC: " << npcName << " | Health: " << health
        << " | Energy: " << energy << " | Money: " << money;
    getDebugConsole().log("NPC", oss.str());
}

void debugSimulationIteration(int iteration, float elapsedTime) {
    std::ostringstream oss;
    oss << "Iteration: " << iteration << ", Elapsed Time: " << elapsedTime << "s";
    getDebugConsole().log("Simulation", oss.str());
}

void debugMarketTransactions(const std::unordered_map<std::string, int>& transactions) {
    std::ostringstream oss;
    oss << "Market Transactions: ";
    for (const auto& [item, count] : transactions) {
        oss << item << ": " << count << " ";
    }
    getDebugConsole().log("Market", oss.str());
}
