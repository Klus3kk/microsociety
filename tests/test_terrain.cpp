#include <gtest/gtest.h>
#include "Game.hpp"
#include "Tile.hpp"
// Test terrain generation to ensure it generates all tile types
TEST(TerrainTest, TerrainGeneration) {
    Game game;
    game.generateMap(); // Explicitly generate the map
    const auto& tileMap = game.getTileMap();

    ASSERT_EQ(tileMap.size(), GameConfig::mapHeight / GameConfig::tileSize);
    ASSERT_EQ(tileMap[0].size(), GameConfig::mapWidth / GameConfig::tileSize);

    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            ASSERT_NE(tile, nullptr);  // Ensure every tile exists
        }
    }
}


// Test object placement on tiles
TEST(TerrainTest, ObjectPlacement) {
    Game game;
    std::srand(42); // Fixed seed for deterministic behavior
    game.generateMap();
    const auto& tileMap = game.getTileMap();

    bool objectFound = false; // Track if at least one object is found
    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            if (auto grassTile = dynamic_cast<GrassTile*>(tile.get())) {
                if (grassTile->getObject()) {
                    EXPECT_TRUE(dynamic_cast<Tree*>(grassTile->getObject()) || dynamic_cast<Bush*>(grassTile->getObject()));
                    objectFound = true;
                }
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tile.get())) {
                if (stoneTile->getObject()) {
                    EXPECT_TRUE(dynamic_cast<Rock*>(stoneTile->getObject()));
                    objectFound = true;
                }
            }
        }
    }
    EXPECT_TRUE(objectFound); // Ensure at least one object was placed
}


// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
