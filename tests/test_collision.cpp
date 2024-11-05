#include <gtest/gtest.h>
#include "Game.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Object.hpp" 

// Test that an NPC collides with a static object (e.g., tree)
TEST(CollisionTest, NPCTreeCollision) {
    Game game;
    PlayerEntity npc(100, 50, 50, 150.0f, 10, 100);
    game.generateMap();

    // Load a texture for the tree (use an actual path or a mock)
    sf::Texture treeTexture;
    treeTexture.loadFromFile("../assets/objects/tree1.png");  // Replace with a valid path

    auto tree = std::make_unique<Tree>(treeTexture);
    game.getTileMap()[5][5]->placeObject(std::move(tree));

    npc.setPosition(5 * game.tileSize, 5 * game.tileSize); // Move NPC to tree's position
    bool collisionOccurred = game.detectCollision(npc);
    EXPECT_TRUE(collisionOccurred);
}

// Test that an NPC can move freely when there is no object in the path
TEST(CollisionTest, NPCFreeMovement) {
    Game game;
    PlayerEntity npc(100, 50, 50, 150.0f, 10, 100);
    game.generateMap();

    // Move NPC to an empty tile (no object)
    npc.setPosition(10 * game.tileSize, 10 * game.tileSize);
    bool collisionOccurred = game.detectCollision(npc);
    EXPECT_FALSE(collisionOccurred);
}

// Edge case: Test collision near the boundary of a tile
TEST(CollisionTest, NPCBoundaryCollision) {
    Game game;
    PlayerEntity npc(100, 50, 50, 150.0f, 10, 100);
    game.generateMap();

    // Place an object near the edge to test boundary collision
    auto tree = std::make_unique<Tree>(/* pass any mock texture if needed */);
    game.getTileMap()[8][8]->placeObject(std::move(tree));

    // Place NPC near the boundary of tile (8,8) to test collision
    npc.setPosition(8 * game.tileSize + game.tileSize - 1, 8 * game.tileSize);
    bool collisionOccurred = game.detectCollision(npc);
    EXPECT_TRUE(collisionOccurred);
}

// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
