#include <gtest/gtest.h>

#include "core/Card.hpp"
#include "core/PlayerHand.hpp"

TEST(PlayerHand, FullAndEmpty) {
    PlayerHand hand;
    EXPECT_TRUE(hand.IsEmpty());

    hand.AddCard({ESuit::CLUBS, ERank::ACE});
    hand.AddCard({ESuit::CLUBS, ERank::EIGHT});
    
    EXPECT_TRUE(hand.IsFull());

    hand.Clear();
    EXPECT_TRUE(hand.IsEmpty());
}