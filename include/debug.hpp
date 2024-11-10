#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <vector>
#include "Player.hpp"
#include "Configuration.hpp"
void debugPlayerInfo(const PlayerEntity& player) {
    std::cout << "Player Position: (" << player.getPosition().x << ", " << player.getPosition().y << ")\n";
    std::cout << "Player Speed: " << player.getSpeed() << "\n";
    std::cout << "Player Health: " << player.getHealth() << "\n";
}

void debugMapInfo(const Game& game) {
    std::cout << "Map Width: " << GameConfig::mapWidth << ", Map Height: " << GameConfig::mapHeight << "\n";
    std::cout << "Tile Size: " << GameConfig::tileSize << "\n";
    std::cout << "Map Layout:\n";
    
    for (int y = 0; y < game.getTileMap().size(); ++y) {
        for (int x = 0; x < game.getTileMap()[y].size(); ++x) {
            std::cout << (game.getTileMap()[y][x]->hasObject() ? "O" : ".") << " ";
        }
        std::cout << "\n";
    }
}

void debugTileInfo(int tileX, int tileY, const Game& game) {
    if (tileX >= 0 && tileX < game.getTileMap()[0].size() && tileY >= 0 && tileY < game.getTileMap().size()) {
        auto& tile = game.getTileMap()[tileY][tileX];
        std::cout << "Tile (" << tileX << ", " << tileY << ") ";
        if (tile->hasObject()) {
            std::cout << "contains object";
            auto objectBounds = tile->getObjectBounds();
            std::cout << " with bounds (" << objectBounds.left << ", " << objectBounds.top << ", "
                      << objectBounds.width << ", " << objectBounds.height << ")\n";
        } else {
            std::cout << "is empty\n";
        }
    } else {
        std::cout << "Tile out of bounds\n";
    }
}

void debugObjectBoundaries(const Game& game) {
    for (int y = 0; y < game.getTileMap().size(); ++y) {
        for (int x = 0; x < game.getTileMap()[y].size(); ++x) {
            auto& tile = game.getTileMap()[y][x];
            if (tile->hasObject()) {
                auto objectBounds = tile->getObjectBounds();
                std::cout << "Object at Tile (" << x << ", " << y << ") with bounds ("
                          << objectBounds.left << ", " << objectBounds.top << ", "
                          << objectBounds.width << ", " << objectBounds.height << ")\n";
            }
        }
    }
}

#endif
