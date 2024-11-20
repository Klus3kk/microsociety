#include "debug.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <algorithm> // For std::max

// DebugConsole implementation
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

void DebugConsole::log(const std::string& message) {
    logs.push_back(message);
    if (logs.size() > 50) logs.erase(logs.begin()); // Limit total log size
}

void DebugConsole::render(sf::RenderWindow& window) {
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

void DebugConsole::scrollUp() { /* Removed scrollOffset as it's unused */ }
void DebugConsole::scrollDown() { /* Removed scrollOffset as it's unused */ }

// Singleton instance
DebugConsole& getDebugConsole() {
    static DebugConsole instance(GameConfig::mapWidth, GameConfig::mapHeight);
    return instance;
}

// Helper functions to throttle logs
bool shouldThrottleLog(std::chrono::high_resolution_clock::time_point& lastLogTime, int ms) {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLogTime).count();
    if (elapsed > ms) {
        lastLogTime = now;
        return false;
    }
    return true;
}

void debugPlayerInfo(const PlayerEntity& player) {
    static sf::Vector2f lastPosition(-1, -1);
    if (player.getPosition() != lastPosition) {
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
        lastPosition = player.getPosition();
    }
}

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
        getDebugConsole().log(oss.str());
        lastTileLog = {tileX, tileY};
    }
}

void debugMarketPrices(const std::unordered_map<std::string, float>& marketPrices) {
    static auto lastMarketLogTime = std::chrono::high_resolution_clock::now();
    if (!shouldThrottleLog(lastMarketLogTime, 1000)) {
        std::ostringstream oss;
        oss << "Market Prices:\n";
        for (const auto& [resource, price] : marketPrices) {
            oss << "- " << resource << ": $" << price << "\n";
        }
        getDebugConsole().log(oss.str());
    }
}

void debugCollisionEvent(const PlayerEntity& player, int tileX, int tileY) {
    std::ostringstream oss;
    oss << "Collision detected! Player at (" << player.getPosition().x << ", " << player.getPosition().y << ")";
    oss << " collided with object on Tile (" << tileX << ", " << tileY << ").";
    getDebugConsole().log(oss.str());
}

void debugActionPerformed(const std::string& actionName, const std::string& objectType) {
    std::ostringstream oss;
    oss << "Action Performed: " << actionName << " on " << objectType << ".";
    getDebugConsole().log(oss.str());
}
