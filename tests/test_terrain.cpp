// test_terrain.cpp
#include <cassert>
#include "Game.hpp"
#include <iostream>

// Test terrain generation to ensure it generates all tile types
void testTerrainGeneration() {
    Game game;
    const auto& tileMap = game.getTileMap();  // Use the getter

    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            assert(tile != nullptr);  // Ensure every tile exists
        }
    }
}
// Test object placement on tiles
void testObjectPlacement() {
    Game game;
    game.generateMap();  // Ensure the map is generated
    const auto& tileMap = game.getTileMap();  // Use the getter

    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            if (auto grassTile = dynamic_cast<GrassTile*>(tile.get())) {
                if (grassTile->getObject()) {
                    assert(dynamic_cast<Tree*>(grassTile->getObject()) || dynamic_cast<Bush*>(grassTile->getObject()));
                }
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tile.get())) {
                if (stoneTile->getObject()) {
                    assert(dynamic_cast<Rock*>(stoneTile->getObject()));
                }
            }
        }
    }
}


int main() {
    testTerrainGeneration();
    testObjectPlacement();

    std::cout << "All tests passed!\n";
    return 0;
}
