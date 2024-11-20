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

TEST(TradingTest, DynamicPricing) {
    Market market;

    // Initial prices
    market.setPrice("wood", 10);
    market.sellItem("wood", 5); // Increase demand
    EXPECT_GT(market.getPrice("wood"), 10); // Expect price to rise
}
