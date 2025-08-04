#include <gtest/gtest.h>
#include "table/PlayerSession.hpp"
#include "core/Card.hpp"

TEST(PlayerSessionTest, NewHandInitializesState) {
    PlayerSession session;
    // All state is reset
    EXPECT_FALSE(session.IsFold());
    EXPECT_DOUBLE_EQ(session.GetLastBet(), 0.0);
    EXPECT_EQ(session.GetHand().size(), 2);
    EXPECT_EQ(session.GetHand()[0], Card{}); // default Card (assuming no value set)
    EXPECT_EQ(session.GetHand()[1], Card{}); // default Card
}

TEST(PlayerSessionTest, AddCardAndClearHand) {
    PlayerSession session;
    Card c1{ECardSuit::HEARTS, ECardRank::ACE};
    Card c2{ECardSuit::SPADES, ECardRank::KING};
    Card c3{ECardSuit::DIAMONDS, ECardRank::QUEEN}; // Should not be added

    EXPECT_TRUE(session.AddCard(c1));
    EXPECT_TRUE(session.AddCard(c2));
    EXPECT_FALSE(session.AddCard(c3)); // hand is full

    const auto& hand = session.GetHand();
    EXPECT_EQ(hand[0], c1);
    EXPECT_EQ(hand[1], c2);

    session.ClearHand();
    // After ClearHand, count is zero but hand still holds previous cards (unless overwritten)
    // Add new card after clear
    Card c4{ECardSuit::CLUBS, ECardRank::TEN};
    EXPECT_TRUE(session.AddCard(c4));
    EXPECT_EQ(session.GetHand()[0], c4);
}

TEST(PlayerSessionTest, FoldAndBetLogic) {
    PlayerSession session;
    EXPECT_FALSE(session.IsFold());
    session.SetFold(true);
    EXPECT_TRUE(session.IsFold());

    session.SetLastBet(25.5);
    EXPECT_DOUBLE_EQ(session.GetLastBet(), 25.5);

    session.SetLastBet(0.0);
    session.SetFold(false);

    EXPECT_DOUBLE_EQ(session.GetLastBet(), 0.0);
    EXPECT_FALSE(session.IsFold());
}
