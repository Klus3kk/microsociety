#include <gtest/gtest.h>
#include "Actions.hpp"
#include "NPCEntity.hpp"
#include "Tile.hpp"

TEST(ActionTest, TreeActionTest) {
    NPCEntity player("Player1", 100, 50, 50, 150.0f, 10, 100);

    TreeAction treeAction;

    // Mock tile with a tree object
    sf::Texture treeTexture;
    treeTexture.loadFromFile("../assets/objects/tree1.png"); // Load a sample texture for testing
    Tile tile;
    tile.placeObject(std::make_unique<Tree>(treeTexture)); // Place a tree on the tile

    // Create a minimal tileMap with a single Tile
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap(1);
    tileMap[0].push_back(std::make_unique<Tile>());
    tileMap[0][0]->placeObject(std::make_unique<Tree>(treeTexture));  // Place the tree object in the new tile

    // Perform the action with the updated tileMap
    treeAction.perform(player, *tileMap[0][0], tileMap);

    // Check if "wood" was added to the inventory
    EXPECT_EQ(player.getInventoryItemCount("wood"), 1);  

    // Verify that the object was removed from the tile
    EXPECT_FALSE(tile.hasObject());
}

// Unit test for StoneAction
TEST(ActionTest, StoneActionTest) {
    NPCEntity player("Player1", 100, 50, 50, 150.0f, 10, 100);

    StoneAction stoneAction;

    // Mock tile with a stone object
    sf::Texture stoneTexture;
    stoneTexture.loadFromFile("../assets/objects/rock1.png"); // Load a sample texture for testing
    Tile tile;
    tile.placeObject(std::make_unique<Rock>(stoneTexture)); // Place a rock on the tile

    // Create a minimal tileMap with a single Tile
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap(1);
    tileMap[0].push_back(std::make_unique<Tile>());
    tileMap[0][0]->placeObject(std::make_unique<Rock>(stoneTexture));  // Place the rock object in the new tile

    // Perform the action with the updated tileMap
    stoneAction.perform(player, *tileMap[0][0], tileMap);

    // Check if "stone" was added to the inventory
    EXPECT_EQ(player.getInventoryItemCount("stone"), 1); 

    // Verify that the object was removed from the tile
    EXPECT_FALSE(tile.hasObject());
}

TEST(ActionTest, BushActionTest) {
    NPCEntity player("Player1", 100, 50, 50, 150.0f, 10, 100);

    BushAction bushAction;

    // Mock tile with a bush object
    sf::Texture bushTexture;
    bushTexture.loadFromFile("../assets/objects/bush1.png"); // Load a sample texture for testing
    Tile tile;
    tile.placeObject(std::make_unique<Bush>(bushTexture)); // Place a bush on the tile

    // Create a minimal tileMap with a single Tile
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap(1);
    tileMap[0].push_back(std::make_unique<Tile>());
    tileMap[0][0]->placeObject(std::make_unique<Bush>(bushTexture));  // Place the bush object in the new tile

    // Perform the action with the updated tileMap
    bushAction.perform(player, *tileMap[0][0], tileMap);

    // Check if "food" was added to the inventory
    EXPECT_EQ(player.getInventoryItemCount("food"), 1);  

    // Verify that the object was removed from the tile
    EXPECT_FALSE(tile.hasObject());
}
