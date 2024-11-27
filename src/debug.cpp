#include "debug.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <algorithm>

DebugConsole::DebugConsole(float windowWidth, float windowHeight) {
    if (!consoleFont.loadFromFile("../assets/fonts/font.ttf")) {
        std::cerr << "Failed to load console font!" << std::endl;
    }
    text.setFont(consoleFont);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::White);

    background.setSize(sf::Vector2f(windowWidth, 200));
    background.setFillColor(backgroundColor);
    background.setPosition(0, windowHeight - 200);
}

void DebugConsole::toggle() { enabled = !enabled; }
void DebugConsole::enable() { enabled = true; }
void DebugConsole::disable() { enabled = false; }
bool DebugConsole::isEnabled() const { return enabled; }


void DebugConsole::setLogLevel(LogLevel level) {
    filterLevel = level;
}

void DebugConsole::log(const std::string& category, const std::string& message, LogLevel level) {
    if (level < filterLevel) return; // Filter out lower-severity logs

    std::ostringstream formattedMessage;
    switch (level) {
        case LogLevel::Info: formattedMessage << "[INFO] "; break;
        case LogLevel::Warning: formattedMessage << "[WARNING] "; break;
        case LogLevel::Error: formattedMessage << "[ERROR] "; break;
    }
    formattedMessage << "[" << category << "] " << message;

    logs.emplace_back(category, formattedMessage.str());
    if (logs.size() > maxLogs) logs.erase(logs.begin());
}

void DebugConsole::logThrottled(const std::string& category, const std::string& message, int throttleMs) {
    auto now = std::chrono::high_resolution_clock::now();
    std::string key = category + ":" + message;
    auto& lastLogTime = throttleTimers[key];
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLogTime).count();

    if (elapsed > throttleMs) {
        log(category, message, LogLevel::Info);
        lastLogTime = now;
    }
}

void DebugConsole::logOnce(const std::string& category, const std::string& message) {
    std::string key = category + ":" + message;
    if (!logOnceTracker[key]) {
        log(category, message, LogLevel::Info);
        logOnceTracker[key] = true;
    }
}

void DebugConsole::logSystemStats(float fps, size_t memoryUsage) {
    std::ostringstream oss;
    oss << "FPS: " << std::fixed << std::setprecision(2) << fps
        << ", Memory Usage: " << memoryUsage << " KB";
    log("System", oss.str(), LogLevel::Info);
}

void DebugConsole::saveLogsToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to save logs to " << filename << std::endl;
        return;
    }
    for (const auto& [category, message] : logs) {
        outFile << message << "\n";
    }
    std::cout << "Logs saved to " << filename << std::endl;
}

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

void DebugConsole::clearLogs() {
    logs.clear();
    // std::cerr << "Debug logs cleared." << std::endl;
}

// Singleton instance
DebugConsole& getDebugConsole() {
    static DebugConsole instance(GameConfig::windowWidth, GameConfig::windowHeight);
    return instance;
}

// Debug helper functions
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

void debugMarketPrices(const std::unordered_map<std::string, float>& marketPrices) {
    static auto lastMarketLogTime = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMarketLogTime).count();

    if (elapsed > 1000) { // 1-second throttle
        std::ostringstream oss;
        oss << "Market Prices:\n";
        for (const auto& [resource, price] : marketPrices) {
            oss << "- " << resource << ": $" << price << "\n";
        }
        getDebugConsole().log("Market", oss.str());
        lastMarketLogTime = now;
    }
}

void debugCollisionEvent(const std::string& message, int throttleMs) {
    getDebugConsole().logThrottled("Collision", message, throttleMs);
}

void debugActionPerformed(const std::string& actionName, const std::string& objectType) {
    std::ostringstream oss;
    oss << "Action: " << actionName << " on " << objectType;
    getDebugConsole().log("Action", oss.str());
}

void debugPlayerSpeed(float speed) {
    static float lastLoggedSpeed = -1.0f;
    if (speed != lastLoggedSpeed) {
        getDebugConsole().log("Player", "Speed set to: " + std::to_string(speed));
        lastLoggedSpeed = speed;
    }
}
