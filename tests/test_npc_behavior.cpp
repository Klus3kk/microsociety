#include <gtest/gtest.h>
#include "Player.hpp"
#include "Game.hpp"
#include <string>

// Test NPC Initialization
TEST(NPCBehaviorTest, NPCInitialization) {
    PlayerEntity npc("Player1",100, 50, 100, 1.5f, 15, 500);
    EXPECT_FLOAT_EQ(npc.getHealth(), 100);
    EXPECT_FLOAT_EQ(npc.getHunger(), 50);
    EXPECT_FLOAT_EQ(npc.getEnergy(), 100);
    EXPECT_FLOAT_EQ(npc.getSpeed(), 1.5f);
    EXPECT_EQ(npc.getStrength(), 15);
    EXPECT_FLOAT_EQ(npc.getMoney(), 500);
}

// Test NPC Adding to Inventory
TEST(NPCBehaviorTest, NPCAddToInventory) {
    PlayerEntity npc("Player1",100, 50, 100, 1.5f, 15, 500);
    bool success = npc.addToInventory("wood", 5);
    EXPECT_TRUE(success);
    EXPECT_EQ(npc.getInventoryItemCount("wood"), 5);

    // Fill up inventory and test failure case
    success = npc.addToInventory("stone", 6);
    EXPECT_FALSE(success);
    EXPECT_EQ(npc.getInventoryItemCount("stone"), 0);
}

// Test NPC Removing from Inventory
TEST(NPCBehaviorTest, NPCRemoveFromInventory) {
    PlayerEntity npc("Player1",100, 50, 100, 1.5f, 15, 500);
    npc.addToInventory("food", 3);

    bool success = npc.removeFromInventory("food", 2);
    EXPECT_TRUE(success);
    EXPECT_EQ(npc.getInventoryItemCount("food"), 1);

    success = npc.removeFromInventory("food", 2); // Trying to remove more than available
    EXPECT_FALSE(success);
    EXPECT_EQ(npc.getInventoryItemCount("food"), 1);
}

// Test NPC Interaction
TEST(NPCBehaviorTest, NPCInteraction) {
    PlayerEntity npc1("Player1",100, 50, 100, 1.5f, 15, 500);
    PlayerEntity npc2("Player2",100, 50, 100, 1.5f, 15, 500);

    npc1.addToInventory("gold", 3);
    EXPECT_EQ(npc1.getInventoryItemCount("gold"), 3);

    npc2.addToInventory("silver", 2);
    EXPECT_EQ(npc2.getInventoryItemCount("silver"), 2);

    // Simulate trade or interaction here if needed
}

// Main function for running all tests
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
