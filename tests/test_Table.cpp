#include <gtest/gtest.h>
#include "table/Table.hpp"
#include "core/Card.hpp"

TEST(TableTest, InitializationAndBlinds) {
    Table table(1.5, 3.0);
    EXPECT_DOUBLE_EQ(table.GetBlindSmall(), 1.5);
    EXPECT_DOUBLE_EQ(table.GetBlindBig(), 3.0);
    EXPECT_DOUBLE_EQ(table.GetPot(), 0.0);

    table.SetBlindSmall(2.0);
    table.SetBlindBig(4.0);
    EXPECT_DOUBLE_EQ(table.GetBlindSmall(), 2.0);
    EXPECT_DOUBLE_EQ(table.GetBlindBig(), 4.0);
}

TEST(TableTest, PotIncreaseAndCollect) {
    /*Table table(1.0, 2.0);

    table.IncreasePot(5.0);
    EXPECT_DOUBLE_EQ(table.GetPot(), 5.0);
    table.IncreasePot(7.5);
    EXPECT_DOUBLE_EQ(table.GetPot(), 12.5);

    // CollectPot returns all and resets to zero
    Coins_t pot = table.CollectPot();
    EXPECT_DOUBLE_EQ(pot, 12.5);
    EXPECT_DOUBLE_EQ(table.GetPot(), 0.0);

    // CollectPot again returns zero
    EXPECT_DOUBLE_EQ(table.CollectPot(), 0.0);*/
}

TEST(TableTest, AddAndGetCommunityCards) {
    Table table(1.0, 2.0);
    Card c1{ECardSuit::HEARTS, ECardRank::ACE};
    Card c2{ECardSuit::SPADES, ECardRank::KING};

    EXPECT_TRUE(table.GetCommunityCards().empty());
    table.AddCommunityCard(c1);
    table.AddCommunityCard(c2);

    const auto& comm = table.GetCommunityCards();
    ASSERT_EQ(comm.size(), 2);
    EXPECT_EQ(comm[0], c1);
    EXPECT_EQ(comm[1], c2);
}
