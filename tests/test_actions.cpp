#include <gtest/gtest.h>
#include "Actions.hpp"
#include "Player.hpp"
#include "Tile.hpp"

TEST(ActionTest, TreeActionTest) {
    PlayerEntity player("Player1",100, 50, 50, 150.0f, 10, 100);

    TreeAction treeAction;

    // Mock tile with a tree object
    sf::Texture treeTexture;
    treeTexture.loadFromFile("../assets/objects/tree1.png"); // Load a sample texture for testing
    Tile tile;
    tile.placeObject(std::make_unique<Tree>(treeTexture)); // Place a tree on the tile

    // Perform the action
    treeAction.perform(player, tile);

    // Check if "wood" was added to the inventory
    EXPECT_EQ(player.getInventoryItemCount("wood"), 1);  

    // Verify that the object was removed from the tile
    EXPECT_FALSE(tile.hasObject());
}

// Unit test for StoneAction
TEST(ActionTest, StoneActionTest) {
    PlayerEntity player("Player1",100, 50, 50, 150.0f, 10, 100);

    StoneAction stoneAction;

    // Mock tile with a stone object
    sf::Texture stoneTexture;
    stoneTexture.loadFromFile("../assets/objects/rock1.png"); // Load a sample texture for testing
    Tile tile;
    tile.placeObject(std::make_unique<Rock>(stoneTexture)); // Place a rock on the tile

    // Perform the action
    stoneAction.perform(player, tile);

    // Check if "stone" was added to the inventory
    EXPECT_EQ(player.getInventoryItemCount("stone"), 1); 

    // Verify that the object was removed from the tile
    EXPECT_FALSE(tile.hasObject());
}

TEST(ActionTest, BushActionTest) {
    PlayerEntity player("Player1",100, 50, 50, 150.0f, 10, 100);

    BushAction bushAction;

    // Mock tile with a bush object
    sf::Texture bushTexture;
    bushTexture.loadFromFile("../assets/objects/bush1.png"); // Load a sample texture for testing
    Tile tile;
    tile.placeObject(std::make_unique<Bush>(bushTexture)); // Place a bush on the tile

    // Perform the action
    bushAction.perform(player, tile);

    // Check if "food" was added to the inventory
    EXPECT_EQ(player.getInventoryItemCount("food"), 1);  

    // Verify that the object was removed from the tile
    EXPECT_FALSE(tile.hasObject());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
