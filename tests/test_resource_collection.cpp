#include <gtest/gtest.h>
#include "NPCEntity.hpp"
#include "Tile.hpp"
#include "Actions.hpp"

TEST(ResourceCollectionTest, InventoryFullTest) {
    NPCEntity player("Player1",100, 50, 50, 150.0f, 10, 100);

    // Mock a tile with a tree object
    sf::Texture treeTexture;
    treeTexture.loadFromFile("../assets/objects/tree1.png");
    Tile tile;
    tile.placeObject(std::make_unique<Tree>(treeTexture));

    // Fill the player's inventory to simulate full inventory
    for (int i = 0; i < player.getMaxInventorySize(); ++i) {
        player.addToInventory("wood", 1);
    }

    TreeAction treeAction;

    // Create a minimal tileMap with one tile
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap(1);
    tileMap[0].push_back(std::make_unique<Tile>());
    tileMap[0][0]->placeObject(std::make_unique<Tree>(treeTexture)); // Place the tree object

    // Perform the action with the updated tileMap
    treeAction.perform(player, *tileMap[0][0], tileMap);

    // Ensure the action doesn't proceed when the inventory is full
    EXPECT_EQ(player.getInventoryItemCount("wood"), player.getMaxInventorySize());
    EXPECT_TRUE(tile.hasObject()); // Object remains on tile
}

TEST(ResourceCollectionTest, ObjectRemovedOnCollection) {
    NPCEntity player("Player1",100, 50, 50, 150.0f, 10, 100);

    // Mock a tile with a bush object
    sf::Texture bushTexture;
    bushTexture.loadFromFile("../assets/objects/bush1.png");
    Tile tile;
    tile.placeObject(std::make_unique<Bush>(bushTexture));

    BushAction bushAction;

    // Create a minimal tileMap with one tile
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap(1);
    tileMap[0].push_back(std::make_unique<Tile>());
    tileMap[0][0]->placeObject(std::make_unique<Bush>(bushTexture)); // Place the bush object

    // Perform the action with the updated tileMap
    bushAction.perform(player, *tileMap[0][0], tileMap);

    // Ensure the bush is removed after collection
    EXPECT_EQ(player.getInventoryItemCount("food"), 1);
    EXPECT_FALSE(tile.hasObject());
}
