#include <gtest/gtest.h>
#include "Market.hpp"
#include "NPCEntity.hpp"
TEST(MarketTest, RealisticPriceAdjustment) {
    Market market;
    market.setPrice("wood", 10.0f);

    NPCEntity player("Player", 100, 50, 50, 200.0f, 10, 100);

    // Buy items to increase demand
    for (int i = 0; i < 5; ++i) {
        ASSERT_TRUE(market.buyItem(player, "wood", 1));
    }
    float updatedPrice = market.getPrice("wood");
    EXPECT_GT(updatedPrice, 10.0f) << "Price should increase after buying";

    // Sell items to increase supply
    for (int i = 0; i < 5; ++i) {
        ASSERT_TRUE(market.sellItem(player, "wood", 1));
    }
    updatedPrice = market.getPrice("wood");
    EXPECT_LT(updatedPrice, 10.0f) << "Price should decrease after selling";
}


TEST(MarketTest, PriceAdjustmentOnBuyAndSell) {
    Market market;
    market.setPrice("wood", 10.0f);

    NPCEntity player("TestPlayer", 1000.0f, 50.0f, 50.0f, 1.0f, 10.0f, 1000.0f);

    // Initial price
    float initialPrice = market.getPrice("wood");
    ASSERT_EQ(initialPrice, 10.0f);

    // Buying increases price
    market.buyItem(player, "wood", 10);
    ASSERT_GT(market.getPrice("wood"), initialPrice);

    // Selling decreases price
    float increasedPrice = market.getPrice("wood");
    market.sellItem(player, "wood", 5);
    ASSERT_LT(market.getPrice("wood"), increasedPrice);
}

