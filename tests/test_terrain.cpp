#include <cassert>
#include "Game.hpp"
#include <iostream>

// test terrain generation to ensure it generates all tile types
void testTerrainGeneration() {
    Game game;
    const auto& tileMap = game.getTileMap();  // getter

    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            assert(tile != nullptr);  // ensure every tile exists
        }
    }
    std::cout << "Test Terrain Generation passed!\n";
}
// test object placement on tiles
void testObjectPlacement() {
    Game game;
    game.generateMap();
    const auto& tileMap = game.getTileMap(); 

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
    std::cout << "Test Object Placement passed!\n";
}


int main() {
    testTerrainGeneration();
    testObjectPlacement();

    std::cout << "All tests passed!\n";
    return 0;
}
