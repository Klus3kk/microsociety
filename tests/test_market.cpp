#include <gtest/gtest.h>
#include "Market.hpp"
#include "Player.hpp"

TEST(MarketTest, PriceAdjustment) {
    PlayerEntity player("TestPlayer", 100.0f, 50.0f, 50.0f, 1.0f, 10.0f, 200.0f);
    Market market;
    market.setPrice("wood", 10.0f);

    ASSERT_EQ(market.getPrice("wood"), 10.0f);

    // Perform buying and log changes
    market.buyItem(player, "wood", 5);
    float updatedPrice = market.getPrice("wood");
    ASSERT_GT(updatedPrice, 10.0f) << "Updated Price: " << updatedPrice;

    market.sellItem(player, "wood", 10);
    updatedPrice = market.getPrice("wood");
    ASSERT_LT(updatedPrice, 10.0f) << "Updated Price after sell: " << updatedPrice;
}

TEST(MarketTest, PriceTrendTracking) {
    PlayerEntity player("TestPlayer", 100.0f, 50.0f, 50.0f, 1.0f, 10.0f, 200.0f);
    Market market;
    market.setPrice("stone", 15.0f);

    for (int i = 0; i < 12; ++i) {
        ASSERT_TRUE(market.buyItem(player, "stone", 1)) << "Buy failed at iteration " << i;
    }

    auto trend = market.getPriceTrend("stone");
    ASSERT_EQ(trend.size(), 10) << "Trend Size: " << trend.size();

    float startPrice = trend.front();
    float endPrice = trend.back();
    ASSERT_GT(endPrice, startPrice) << "Start Price: " << startPrice << ", End Price: " << endPrice;
}
