#include <gtest/gtest.h>

#include "core/Card.hpp"
#include "core/Deck.hpp"

#include "utils/random/IRandomProvider.hpp"

// Mock for provider: do shuffle nothing
class MockRandomProvider : public IRandomProvider {
public:
    void Shuffle(std::vector<Card>& cards) override {}
};

// Mock for provider: reverse cards.
class ReverseMockRandomProvider : public IRandomProvider {
public:
    void Shuffle(std::vector<Card>& cards) override {
        std::reverse(cards.begin(), cards.end());
    }
};

TEST(DeckTest, DrawsSequentiallyWithoutShuffle) {
    std::vector<Card> cards = {
        {ESuit::HEARTS, ERank::TWO},
        {ESuit::SPADES, ERank::THREE}
    };
    MockRandomProvider mock_rng;
    Deck deck(cards, mock_rng);

    auto c1 = deck.Draw();
    ASSERT_TRUE(c1.has_value());
    EXPECT_EQ(c1.value(), cards[0]);

    auto c2 = deck.Draw();
    ASSERT_TRUE(c2.has_value());
    EXPECT_EQ(c2.value(), cards[1]);

    auto c3 = deck.Draw();
    EXPECT_FALSE(c3.has_value());
}

TEST(DeckTest, ShuffleCallsRandomProvider) {
    std::vector<Card> cards = {
        {ESuit::HEARTS, ERank::TWO},
        {ESuit::SPADES, ERank::THREE}
    };
    ReverseMockRandomProvider reverse_rng;

    Deck deck(cards, reverse_rng);
    deck.Shuffle();

    auto c1 = deck.Draw();
    ASSERT_TRUE(c1.has_value());
    EXPECT_EQ(c1.value(), cards[1]);

    auto c2 = deck.Draw();
    ASSERT_TRUE(c2.has_value());
    EXPECT_EQ(c2.value(), cards[0]);
}

TEST(DeckTest, GetCardsReflectsCurrentDeckState) {
    std::vector<Card> cards = {
        {ESuit::HEARTS, ERank::TWO},
        {ESuit::SPADES, ERank::THREE}
    };
    MockRandomProvider mock_rng;
    Deck deck(cards, mock_rng);

    EXPECT_EQ(deck.GetCards().size(), 2);
    deck.Draw();
    EXPECT_EQ(deck.GetCards().size(), 2);
}

TEST(DeckTest, ShuffleAfterPartialDraw) {
    std::vector<Card> cards = {
        {ESuit::HEARTS, ERank::TWO},
        {ESuit::SPADES, ERank::THREE},
        {ESuit::DIAMONDS, ERank::FOUR}
    };
    MockRandomProvider mock_rng;
    Deck deck(cards, mock_rng);

    deck.Draw();
    deck.Shuffle();
    auto c1 = deck.Draw();
    auto c2 = deck.Draw();
    auto c3 = deck.Draw();
    EXPECT_TRUE(c1.has_value());
    EXPECT_TRUE(c2.has_value());
    EXPECT_TRUE(c3.has_value());
    EXPECT_FALSE(deck.Draw().has_value());
}