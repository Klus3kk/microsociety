#include <gtest/gtest.h>
#include "Market.hpp"
#include "NPCEntity.hpp"

TEST(TradingTest, BuyItem) {
    Market market;
    NPCEntity player("Player1",100, 50, 50, 150.0f, 10, 200);

    market.setPrice("wood", 10);

    EXPECT_TRUE(market.buyItem(player, "wood", 1));
    EXPECT_EQ(player.getInventoryItemCount("wood"), 1);
    EXPECT_EQ(player.getMoney(), 189); // 200 - 11 (buyMargin applied)
}

TEST(TradingTest, InsufficientFunds) {
    Market market;
    NPCEntity player("Player1",100, 50, 50, 150.0f, 10, 5);

    market.setPrice("stone", 10);

    EXPECT_FALSE(market.buyItem(player, "stone", 1));
    EXPECT_EQ(player.getMoney(), 5);
    EXPECT_EQ(player.getInventoryItemCount("stone"), 0);
}

TEST(TradingTest, SellItem) {
    Market market;
    NPCEntity player("Player1",100, 50, 50, 150.0f, 10, 0);

    market.setPrice("food", 5);
    player.addToInventory("food", 2);

    EXPECT_TRUE(market.sellItem(player, "food", 1));
    EXPECT_EQ(player.getInventoryItemCount("food"), 1); // One removed
    EXPECT_EQ(player.getMoney(), 4.5);                 // Earned 4.5 (sellMargin applied)
}

