#include <gtest/gtest.h>
#include "Game.hpp"
#include "Tile.hpp"
#include "Market.hpp"
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
    game.generateMap(); // Ensure the map is generated
    const auto& tileMap = game.getTileMap();

    int grassTileCount = 0;
    int grassWithObjectCount = 0;

    int stoneTileCount = 0;
    int stoneWithObjectCount = 0;

    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            ASSERT_NE(tile, nullptr);  // Ensure tile exists

            if (auto grassTile = dynamic_cast<GrassTile*>(tile.get())) {
                grassTileCount++;
                if (grassTile->getObject()) {
                    grassWithObjectCount++;
                    EXPECT_TRUE(dynamic_cast<Tree*>(grassTile->getObject()) || dynamic_cast<Bush*>(grassTile->getObject()) ||
                                dynamic_cast<House*>(grassTile->getObject()) || dynamic_cast<Market*>(grassTile->getObject()))
                        << "Invalid object on GrassTile.";
                }
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tile.get())) {
                stoneTileCount++;
                if (stoneTile->getObject()) {
                    stoneWithObjectCount++;
                    EXPECT_TRUE(dynamic_cast<Rock*>(stoneTile->getObject()) ||
                                dynamic_cast<House*>(stoneTile->getObject()) || dynamic_cast<Market*>(stoneTile->getObject()))
                        << "Invalid object on StoneTile.";
                }
            }
        }
    }

    EXPECT_GT(grassWithObjectCount, 0) << "No GrassTiles have objects.";
    EXPECT_GT(stoneWithObjectCount, 0) << "No StoneTiles have objects.";
}






// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
