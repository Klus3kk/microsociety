#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Player.hpp"
#include "Game.hpp"
#include "Configuration.hpp"

class Debug {
private:
    bool debugMode = false;          // Toggle for enabling/disabling debug
    std::vector<std::string> logs;  // Buffer for storing debug messages

public:
    static Debug& getInstance() {
        static Debug instance;
        return instance;
    }

    void enable() { debugMode = true; }
    void disable() { debugMode = false; }
    bool isEnabled() const { return debugMode; }

    void log(const std::string& message) {
        if (debugMode) {
            logs.push_back(message);
            std::cout << message << std::endl; // Print to console for now
        }
    }

    void clearLogs() { logs.clear(); }

    const std::vector<std::string>& getLogs() const { return logs; }

    // Disallow copying
    Debug(const Debug&) = delete;
    Debug& operator=(const Debug&) = delete;

private:
    Debug() = default; // Private constructor for Singleton
};

// Debug functions
inline void debugPlayerInfo(const PlayerEntity& player) {
    std::ostringstream oss;
    oss << "Player Position: (" << player.getPosition().x << ", " << player.getPosition().y << ")\n";
    oss << "Player Speed: " << player.getSpeed() << "\n";
    oss << "Player Health: " << player.getHealth() << "\n";
    Debug::getInstance().log(oss.str());
}

inline void debugMapInfo(const Game& game) {
    std::ostringstream oss;
    oss << "Map Width: " << GameConfig::mapWidth << ", Map Height: " << GameConfig::mapHeight << "\n";
    oss << "Tile Size: " << GameConfig::tileSize << "\n";
    oss << "Map Layout:\n";

    for (int y = 0; y < game.getTileMap().size(); ++y) {
        for (int x = 0; x < game.getTileMap()[y].size(); ++x) {
            oss << (game.getTileMap()[y][x]->hasObject() ? "O" : ".") << " ";
        }
        oss << "\n";
    }
    Debug::getInstance().log(oss.str());
}

inline void debugTileInfo(int tileX, int tileY, const Game& game) {
    if (tileX >= 0 && tileX < game.getTileMap()[0].size() && tileY >= 0 && tileY < game.getTileMap().size()) {
        auto& tile = game.getTileMap()[tileY][tileX];
        std::ostringstream oss;
        oss << "Tile (" << tileX << ", " << tileY << ") ";
        if (tile->hasObject()) {
            oss << "contains object";
            auto objectBounds = tile->getObjectBounds();
            oss << " with bounds (" << objectBounds.left << ", " << objectBounds.top << ", "
                << objectBounds.width << ", " << objectBounds.height << ")\n";
        } else {
            oss << "is empty\n";
        }
        Debug::getInstance().log(oss.str());
    } else {
        Debug::getInstance().log("Tile out of bounds\n");
    }
}

inline void debugObjectBoundaries(const Game& game) {
    std::ostringstream oss;
    for (int y = 0; y < game.getTileMap().size(); ++y) {
        for (int x = 0; x < game.getTileMap()[y].size(); ++x) {
            auto& tile = game.getTileMap()[y][x];
            if (tile->hasObject()) {
                auto objectBounds = tile->getObjectBounds();
                oss << "Object at Tile (" << x << ", " << y << ") with bounds ("
                    << objectBounds.left << ", " << objectBounds.top << ", "
                    << objectBounds.width << ", " << objectBounds.height << ")\n";
            }
        }
    }
    Debug::getInstance().log(oss.str());
}

#endif // DEBUG_HPP
