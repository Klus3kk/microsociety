#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <map>
#include "Configuration.hpp"

// Debug system for centralized logging
class DebugConsole {
private:
    bool enabled = false;                          // Toggle for enabling/disabling debug
    std::vector<std::string> logs;                // Buffer for logs
    sf::Font consoleFont;                         // Font for console
    sf::RectangleShape background;                // Background for console UI
    sf::Text text;                                // Text object for rendering logs
    int maxLogs = 10;                             // Maximum number of logs shown
    float scrollOffset = 0;                       // Scrolling offset for logs
    sf::Color backgroundColor = sf::Color(0, 0, 0, 200); // Opaque black background

public:
    DebugConsole(float windowWidth, float windowHeight) {
        // Load a font for the console (make sure the file exists)
        if (!consoleFont.loadFromFile("../assets/fonts/font.ttf")) {
            std::cerr << "Failed to load console font!" << std::endl;
        }
        text.setFont(consoleFont);
        text.setCharacterSize(14);
        text.setFillColor(sf::Color::White);

        // Initialize the background
        background.setSize(sf::Vector2f(windowWidth, 200)); // Full-width black bar
        background.setFillColor(backgroundColor);
        background.setPosition(0, windowHeight - 200); // Stick to bottom of the screen
    }

    void toggle() { enabled = !enabled; }          // Toggle debug mode
    void enable() { enabled = true; }
    void disable() { enabled = false; }
    bool isEnabled() const { return enabled; }

    void log(const std::string& message) {
        logs.push_back(message);
        if (logs.size() > 50) logs.erase(logs.begin());  // Limit total log size
    }

    void render(sf::RenderWindow& window) {
        if (!enabled) return;
        window.draw(background);

        float yOffset = background.getPosition().y + 10;
        size_t start = logs.size() > maxLogs ? logs.size() - maxLogs : 0;
        for (size_t i = start; i < logs.size(); ++i) {
            text.setString(logs[i]);
            text.setPosition(10, yOffset);
            window.draw(text);
            yOffset += 16;
        }
    }

    void scrollUp() { scrollOffset = std::max(0.0f, scrollOffset - 10.0f); }
    void scrollDown() { scrollOffset += 10.0f; }
};

// Singleton instance
inline DebugConsole& getDebugConsole() {
    static DebugConsole instance(GameConfig::mapWidth, GameConfig::mapHeight);
    return instance;
}

// Debug helper functions
inline void debugPlayerInfo(const PlayerEntity& player) {
    std::ostringstream oss;
    oss << "Player Stats:\n";
    oss << "- Position: (" << player.getPosition().x << ", " << player.getPosition().y << ")\n";
    oss << "- Speed: " << player.getSpeed() << "\n";
    oss << "- Health: " << player.getHealth() << "\n";
    oss << "- Hunger: " << player.getHunger() << "\n";
    oss << "- Energy: " << player.getEnergy() << "\n";
    oss << "- Money: $" << player.getMoney() << "\n";
    oss << "Inventory: ";
    for (const auto& [item, qty] : player.getInventory()) {
        oss << item << " (" << qty << ") ";
    }
    getDebugConsole().log(oss.str());
}

inline void debugTileInfo(int tileX, int tileY, const Game& game) {
    std::ostringstream oss;
    oss << "Tile (" << tileX << ", " << tileY << ") ";
    auto& tile = game.getTileMap()[tileY][tileX];
    if (tile->hasObject()) {
        oss << "contains object";
    } else {
        oss << "is empty";
    }
    getDebugConsole().log(oss.str());
}

inline void debugMarketPrices(const std::unordered_map<std::string, float>& marketPrices) {
    std::ostringstream oss;
    oss << "Market Prices:\n";
    for (const auto& [resource, price] : marketPrices) {
        oss << "- " << resource << ": $" << price << "\n";
    }
    getDebugConsole().log(oss.str());
}

inline void debugCollisionEvent(const PlayerEntity& player, int tileX, int tileY) {
    std::ostringstream oss;
    oss << "Collision detected! Player at (" << player.getPosition().x << ", " << player.getPosition().y << ")";
    oss << " collided with object on Tile (" << tileX << ", " << tileY << ").\n";
    getDebugConsole().log(oss.str());
}

inline void debugActionPerformed(const std::string& actionName, const std::string& objectType) {
    std::ostringstream oss;
    oss << "Action Performed: " << actionName << " on " << objectType << "\n";
    getDebugConsole().log(oss.str());
}


#endif // DEBUG_HPP
