#include <gtest/gtest.h>
#include "NPCEntity.hpp"

TEST(InventoryTest, AddItems) {
    NPCEntity player("Player1",100, 50, 50, 150.0f, 10, 100);

    player.addToInventory("wood", 5);
    EXPECT_EQ(player.getInventoryItemCount("wood"), 5);

    player.addToInventory("stone", 2);
    EXPECT_EQ(player.getInventoryItemCount("stone"), 2);
}

TEST(InventoryTest, RemoveItems) {
    NPCEntity player("Player1",100, 50, 50, 150.0f, 10, 100);

    player.addToInventory("food", 3);
    player.removeFromInventory("food", 1);
    EXPECT_EQ(player.getInventoryItemCount("food"), 2);
}

TEST(InventoryTest, InventoryLimit) {
    NPCEntity player("Player1",100, 50, 50, 150.0f, 10, 100);

    for (int i = 0; i < player.getMaxInventorySize(); ++i) {
        player.addToInventory("wood", 1);
    }


    // Attempt to add beyond limit
    EXPECT_FALSE(player.addToInventory("stone", 1));
    EXPECT_EQ(player.getInventorySize(), player.getMaxInventorySize());
}
