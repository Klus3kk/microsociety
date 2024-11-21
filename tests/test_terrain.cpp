#include <gtest/gtest.h>
#include "Game.hpp"
#include "Tile.hpp"
// Test terrain generation to ensure it generates all tile types
TEST(TerrainTest, TerrainGeneration) {
    Game game;
    const auto& tileMap = game.getTileMap();

    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            ASSERT_NE(tile, nullptr);  // ensure every tile exists
        }
    }
}

// Test object placement on tiles
TEST(TerrainTest, ObjectPlacement) {
    Game game;
    game.generateMap();
    const auto& tileMap = game.getTileMap();

    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            if (auto grassTile = dynamic_cast<GrassTile*>(tile.get())) {
                if (grassTile->getObject()) {
                    EXPECT_TRUE(dynamic_cast<Tree*>(grassTile->getObject()) || dynamic_cast<Bush*>(grassTile->getObject()));
                }
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tile.get())) {
                if (stoneTile->getObject()) {
                    EXPECT_TRUE(dynamic_cast<Rock*>(stoneTile->getObject()));
                }
            }
        }
    }
}

// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
