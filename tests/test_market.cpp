#include <gtest/gtest.h>
#include "Market.hpp"
#include "Player.hpp"

TEST(MarketTest, PriceAdjustment) {
    PlayerEntity player("TestPlayer", 100.0f, 50.0f, 50.0f, 1.0f, 10.0f, 200.0f);
    Market market;
    market.setPrice("wood", 10.0f);

    ASSERT_EQ(market.getPrice("wood"), 10.0f);

    market.buyItem(player, "wood", 5);
    ASSERT_GT(market.getPrice("wood"), 10.0f); // Price should increase

    market.sellItem(player, "wood", 10);
    ASSERT_LT(market.getPrice("wood"), 10.0f); // Price should decrease
}

TEST(MarketTest, PriceTrendTracking) {
    PlayerEntity player("TestPlayer", 100.0f, 50.0f, 50.0f, 1.0f, 10.0f, 200.0f);
    Market market;
    market.setPrice("stone", 15.0f);

    for (int i = 0; i < 12; ++i) {
        market.buyItem(player, "stone", 1);
    }

    auto trend = market.getPriceTrend("stone");
    ASSERT_EQ(trend.size(), 10); // Ensure trend is limited to 10 points
    ASSERT_GT(trend.back(), trend.front()); // Price should increase over time
}
