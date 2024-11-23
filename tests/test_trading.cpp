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

    PlayerEntity player("TestPlayer", 100.0f, 50.0f, 50.0f, 150.0f, 10.0f, 500.0f);

    // Initial price
    float initialPrice = market.getPrice("wood");
    ASSERT_EQ(initialPrice, 10.0f);

    // Buy items to increase demand
    market.buyItem(player, "wood", 1);
    float updatedPrice = market.getPrice("wood");
    EXPECT_GT(updatedPrice, initialPrice);

    // Sell items to increase supply
    market.sellItem(player, "wood", 1);
    float finalPrice = market.getPrice("wood");
    EXPECT_LT(finalPrice, updatedPrice);

    // Log results for debugging
    std::cout << "Initial Price: " << initialPrice
              << ", Updated Price: " << updatedPrice
              << ", Final Price: " << finalPrice << std::endl;
}






