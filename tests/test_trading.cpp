#include <gtest/gtest.h>
#include "Market.hpp"
#include "Player.hpp"

TEST(TradingTest, BuyItem) {
    Market market;
    PlayerEntity player(100, 50, 50, 150.0f, 10, 200);

    market.setPrice("wood", 10);

    EXPECT_TRUE(market.buyItem(player, "wood", 1));
    EXPECT_EQ(player.getInventoryItemCount("wood"), 1);
    EXPECT_EQ(player.getMoney(), 190); // 200 - 10
}

TEST(TradingTest, InsufficientFunds) {
    Market market;
    PlayerEntity player(100, 50, 50, 150.0f, 10, 5);

    market.setPrice("stone", 10);

    EXPECT_FALSE(market.buyItem(player, "stone", 1));
    EXPECT_EQ(player.getMoney(), 5);
    EXPECT_EQ(player.getInventoryItemCount("stone"), 0);
}

TEST(TradingTest, SellItem) {
    Market market;
    PlayerEntity player(100, 50, 50, 150.0f, 10, 0);

    market.setPrice("food", 5);
    player.addToInventory("food", 2);

    EXPECT_TRUE(market.sellItem(player, "food", 1));
    EXPECT_EQ(player.getInventoryItemCount("food"), 1); // One removed
    EXPECT_EQ(player.getMoney(), 5);                   // Earned 5 from sale
}

TEST(TradingTest, DynamicPricing) {
    Market market;
    PlayerEntity player(100, 50, 50, 150.0f, 10, 100); // Create a player

    // Initial prices
    market.setPrice("wood", 10);
    player.addToInventory("wood", 5); // Add items to the player's inventory
    market.sellItem(player, "wood", 5); // Increase supply

    EXPECT_GT(market.getPrice("wood"), 10); // Expect price to increase
}

