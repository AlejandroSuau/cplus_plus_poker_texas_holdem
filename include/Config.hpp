#pragma once

#include "core/Types.hpp"
#include "core/Card.hpp"

#include <raylib.h>

#include <array>
#include <vector>

static constexpr int kTargetFPS = 60;
static constexpr int kScreenWidth = 600;
static constexpr int kScreenHeight = 600;

namespace PlayerAttribs {
static constexpr Vector2 kDimensions {20.f, 20.f};
static constexpr float kSpeed = 10.f;
static constexpr Color kColor = GREEN;
}

static constexpr Coins_t kBlindSmall = 2.0;
static constexpr Coins_t kBlindBig = kBlindSmall * 2;

// TODO: Just to make it easier now to copy. Change to array later.
static const std::vector<Card> kCardDeck {{
    // CLUBS
    {ECardSuit::CLUBS, ECardRank::TWO},
    {ECardSuit::CLUBS, ECardRank::THREE},
    {ECardSuit::CLUBS, ECardRank::FOUR},
    {ECardSuit::CLUBS, ECardRank::FIVE},
    {ECardSuit::CLUBS, ECardRank::SIX},
    {ECardSuit::CLUBS, ECardRank::SEVEN},
    {ECardSuit::CLUBS, ECardRank::EIGHT},
    {ECardSuit::CLUBS, ECardRank::NINE},
    {ECardSuit::CLUBS, ECardRank::TEN},
    {ECardSuit::CLUBS, ECardRank::JACK},
    {ECardSuit::CLUBS, ECardRank::QUEEN},
    {ECardSuit::CLUBS, ECardRank::KING},
    {ECardSuit::CLUBS, ECardRank::ACE},

    // DIAMONDS
    {ECardSuit::DIAMONDS, ECardRank::TWO},
    {ECardSuit::DIAMONDS, ECardRank::THREE},
    {ECardSuit::DIAMONDS, ECardRank::FOUR},
    {ECardSuit::DIAMONDS, ECardRank::FIVE},
    {ECardSuit::DIAMONDS, ECardRank::SIX},
    {ECardSuit::DIAMONDS, ECardRank::SEVEN},
    {ECardSuit::DIAMONDS, ECardRank::EIGHT},
    {ECardSuit::DIAMONDS, ECardRank::NINE},
    {ECardSuit::DIAMONDS, ECardRank::TEN},
    {ECardSuit::DIAMONDS, ECardRank::JACK},
    {ECardSuit::DIAMONDS, ECardRank::QUEEN},
    {ECardSuit::DIAMONDS, ECardRank::KING},
    {ECardSuit::DIAMONDS, ECardRank::ACE},

    // HEARTS
    {ECardSuit::HEARTS, ECardRank::TWO},
    {ECardSuit::HEARTS, ECardRank::THREE},
    {ECardSuit::HEARTS, ECardRank::FOUR},
    {ECardSuit::HEARTS, ECardRank::FIVE},
    {ECardSuit::HEARTS, ECardRank::SIX},
    {ECardSuit::HEARTS, ECardRank::SEVEN},
    {ECardSuit::HEARTS, ECardRank::EIGHT},
    {ECardSuit::HEARTS, ECardRank::NINE},
    {ECardSuit::HEARTS, ECardRank::TEN},
    {ECardSuit::HEARTS, ECardRank::JACK},
    {ECardSuit::HEARTS, ECardRank::QUEEN},
    {ECardSuit::HEARTS, ECardRank::KING},
    {ECardSuit::HEARTS, ECardRank::ACE},

    // SPADES
    {ECardSuit::SPADES, ECardRank::TWO},
    {ECardSuit::SPADES, ECardRank::THREE},
    {ECardSuit::SPADES, ECardRank::FOUR},
    {ECardSuit::SPADES, ECardRank::FIVE},
    {ECardSuit::SPADES, ECardRank::SIX},
    {ECardSuit::SPADES, ECardRank::SEVEN},
    {ECardSuit::SPADES, ECardRank::EIGHT},
    {ECardSuit::SPADES, ECardRank::NINE},
    {ECardSuit::SPADES, ECardRank::TEN},
    {ECardSuit::SPADES, ECardRank::JACK},
    {ECardSuit::SPADES, ECardRank::QUEEN},
    {ECardSuit::SPADES, ECardRank::KING},
    {ECardSuit::SPADES, ECardRank::ACE},
}};
