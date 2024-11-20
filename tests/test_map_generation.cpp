#include <gtest/gtest.h>
#include "Game.hpp"

TEST(MapGenerationTest, MapDimensions) {
    Game game;
    game.generateMap();

    EXPECT_EQ(game.getTileMap().size(), GameConfig::mapHeight);
    EXPECT_EQ(game.getTileMap()[0].size(), GameConfig::mapWidth);
}

TEST(MapGenerationTest, TerrainDistribution) {
    Game game;
    game.generateMap();

    int grassCount = 0, stoneCount = 0;
    for (const auto& row : game.getTileMap()) {
        for (const auto& tile : row) {
            if (dynamic_cast<GrassTile*>(tile.get())) {
                grassCount++;
            } else if (dynamic_cast<StoneTile*>(tile.get())) {
                stoneCount++;
            }
        }
    }

    EXPECT_GT(grassCount, 0);
    EXPECT_GT(stoneCount, 0);
}
