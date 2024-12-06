#include <gtest/gtest.h>
#include "Player.hpp"
#include "Configuration.hpp"

// Test entity creation
TEST(EntityTest, EntityCreation) {
    PlayerEntity player("Player1", 100, 50, 100, 2.0f, 10, 100);

    EXPECT_EQ(player.getHealth(), 100);
    EXPECT_EQ(player.getHunger(), 50);
    EXPECT_EQ(player.getEnergy(), 100);
    EXPECT_FLOAT_EQ(player.getSpeed(), 2.0f);
    EXPECT_EQ(player.getStrength(), 10);
    EXPECT_EQ(player.getMoney(), 100);
}

// Test entity movement
TEST(EntityTest, EntityMovement) {
    PlayerEntity player("Player1", 100, 50, 100, 2.0f, 10, 100);
    player.setPosition(0, 0);

    float deltaTime = 1.0f;
    player.move(1, 0, deltaTime);
    EXPECT_FLOAT_EQ(player.getPosition().x, 2.0f * deltaTime);  // movement based on speed and deltaTime

    player.move(0, 1, deltaTime);
    EXPECT_FLOAT_EQ(player.getPosition().y, 2.0f * deltaTime);
}

// Main function to run all tests
// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
