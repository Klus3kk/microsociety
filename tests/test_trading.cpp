#include <gtest/gtest.h>
#include "Market.hpp"
#include "Player.hpp"

TEST(TradingTest, BuyItem) {
    Market market;
    PlayerEntity player("Player1",100, 50, 50, 150.0f, 10, 200);

    market.setPrice("wood", 10);

    EXPECT_TRUE(market.buyItem(player, "wood", 1));
    EXPECT_EQ(player.getInventoryItemCount("wood"), 1);
    EXPECT_EQ(player.getMoney(), 190); // 200 - 10
}

TEST(TradingTest, InsufficientFunds) {
    Market market;
    PlayerEntity player("Player1",100, 50, 50, 150.0f, 10, 5);

    market.setPrice("stone", 10);

    EXPECT_FALSE(market.buyItem(player, "stone", 1));
    EXPECT_EQ(player.getMoney(), 5);
    EXPECT_EQ(player.getInventoryItemCount("stone"), 0);
}

TEST(TradingTest, SellItem) {
    Market market;
    PlayerEntity player("Player1",100, 50, 50, 150.0f, 10, 0);

    market.setPrice("food", 5);
    player.addToInventory("food", 2);

    EXPECT_TRUE(market.sellItem(player, "food", 1));
    EXPECT_EQ(player.getInventoryItemCount("food"), 1); // One removed
    EXPECT_EQ(player.getMoney(), 5);                   // Earned 5 from sale
}

TEST(TradingTest, DynamicPricing) {
    Market market;
    market.setPrice("wood", 10.0f);

    // Player setup
    PlayerEntity player("TestPlayer", 100, 50, 50, 150.0f, 10, 100);
    player.setMoney(500); // Give player enough money

    // Buy items to increase demand
    market.buyItem(player, "wood", 5);
    float updatedPrice = market.getPrice("wood");

    EXPECT_GT(updatedPrice, 10.0f) << "Price for wood did not increase as expected!";

    // Sell items to increase supply
    market.sellItem(player, "wood", 3);
    updatedPrice = market.getPrice("wood");

    EXPECT_LT(updatedPrice, 10.0f) << "Price for wood did not decrease as expected!";
}




