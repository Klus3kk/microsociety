#include <gtest/gtest.h>
#include "Actions.hpp"
#include "Player.hpp"

TEST(ActionTest, TreeActionTest) {
    PlayerEntity player(100, 50, 50, 150.0f, 10, 100);

    TreeAction treeAction;
    treeAction.perform(player);

    // Check if "wood" was added to the inventory
    auto inventory = player.getInventory();
    EXPECT_EQ(inventory.at("wood"), 1); 
}

TEST(ActionTest, StoneActionTest) {
    PlayerEntity player(100, 50, 50, 150.0f, 10, 100);

    StoneAction stoneAction;
    stoneAction.perform(player);

    // Check if "stone" was added to the inventory
    auto inventory = player.getInventory();
    EXPECT_EQ(inventory.at("stone"), 1);  
}

TEST(ActionTest, BushActionTest) {
    PlayerEntity player(100, 50, 50, 150.0f, 10, 100);

    BushAction bushAction;
    bushAction.perform(player);

    // Check if "food" was added to the inventory
    auto inventory = player.getInventory();
    EXPECT_EQ(inventory.at("food"), 1);  
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
