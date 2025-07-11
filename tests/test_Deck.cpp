#include <gtest/gtest.h>

#include "core/Card.hpp"
#include "core/Deck.hpp"

TEST(DeckTest, Draw) {
    Deck::DeckCards_t deck_cards {
        {ESuit::HEARTS, ERank::TWO},
        {ESuit::HEARTS, ERank::THREE},
        {ESuit::HEARTS, ERank::FOUR},
        {ESuit::HEARTS, ERank::ACE}
    };
    Deck deck(deck_cards);

    EXPECT_EQ(deck.Draw(), deck_cards[0]);
}

TEST(DeckTest, Shuffle) {
    Deck::DeckCards_t deck_cards {
        {ESuit::HEARTS, ERank::TWO},
        {ESuit::HEARTS, ERank::THREE},
        {ESuit::HEARTS, ERank::FOUR},
        {ESuit::HEARTS, ERank::ACE}
    };
    Deck deck(deck_cards);

    EXPECT_EQ(deck.GetCards(), deck_cards);

    deck.Shuffle();
    
    EXPECT_NE(deck.GetCards(), deck_cards);
}