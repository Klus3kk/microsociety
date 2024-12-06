#include <gtest/gtest.h>
#include "Game.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Object.hpp"
#include <SFML/Graphics.hpp>
#include "Configuration.hpp"

// Test that an NPC collides with a static object (e.g., tree)
TEST(CollisionTest, NPCTreeCollision) {
    Game game;
    PlayerEntity npc("Player1",100, 50, 50, 150.0f, 10, 100);
    game.generateMap();

    // Load a texture for the tree
    sf::Texture treeTexture;
    ASSERT_TRUE(treeTexture.loadFromFile("../assets/objects/tree1.png")) << "Failed to load tree texture";

    auto tree = std::make_unique<Tree>(treeTexture);
    game.getTileMap()[5][5]->placeObject(std::move(tree));

    npc.setPosition(5 * GameConfig::tileSize, 5 * GameConfig::tileSize); // Move NPC to tree's position
    bool collisionOccurred = game.detectCollision(npc);
    EXPECT_FALSE(collisionOccurred);
}

// Test that an NPC can move freely when there is no object in the path
TEST(CollisionTest, NPCFreeMovement) {
    Game game;
    PlayerEntity npc("Player1",100, 50, 50, 150.0f, 10, 100);
    game.generateMap();

    // Move NPC to an empty tile (no object)
    npc.setPosition(10 * GameConfig::tileSize, 10 * GameConfig::tileSize);
    bool collisionOccurred = game.detectCollision(npc);
    EXPECT_FALSE(collisionOccurred);
}

// Edge case: Test collision near the boundary of a tile
TEST(CollisionTest, NPCBoundaryCollision) {
    Game game;
    PlayerEntity npc("Player1",100, 50, 50, 150.0f, 10, 100);
    game.generateMap();

    // Load a texture for the tree
    sf::Texture treeTexture;
    ASSERT_TRUE(treeTexture.loadFromFile("../assets/objects/tree1.png")) << "Failed to load tree texture";

    auto tree = std::make_unique<Tree>(treeTexture);
    game.getTileMap()[8][8]->placeObject(std::move(tree));

    // Place NPC near the boundary of tile (8,8) to test collision
    npc.setPosition(8 * GameConfig::tileSize + GameConfig::tileSize - 1, 8 * GameConfig::tileSize);
    bool collisionOccurred = game.detectCollision(npc);
    EXPECT_FALSE(collisionOccurred);
}

// Main function to run all tests
// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
