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
    {ESuit::CLUBS, ERank::TWO},
    {ESuit::CLUBS, ERank::THREE},
    {ESuit::CLUBS, ERank::FOUR},
    {ESuit::CLUBS, ERank::FIVE},
    {ESuit::CLUBS, ERank::SIX},
    {ESuit::CLUBS, ERank::SEVEN},
    {ESuit::CLUBS, ERank::EIGHT},
    {ESuit::CLUBS, ERank::NINE},
    {ESuit::CLUBS, ERank::TEN},
    {ESuit::CLUBS, ERank::JACK},
    {ESuit::CLUBS, ERank::QUEEN},
    {ESuit::CLUBS, ERank::KING},
    {ESuit::CLUBS, ERank::ACE},

    // DIAMONDS
    {ESuit::DIAMONDS, ERank::TWO},
    {ESuit::DIAMONDS, ERank::THREE},
    {ESuit::DIAMONDS, ERank::FOUR},
    {ESuit::DIAMONDS, ERank::FIVE},
    {ESuit::DIAMONDS, ERank::SIX},
    {ESuit::DIAMONDS, ERank::SEVEN},
    {ESuit::DIAMONDS, ERank::EIGHT},
    {ESuit::DIAMONDS, ERank::NINE},
    {ESuit::DIAMONDS, ERank::TEN},
    {ESuit::DIAMONDS, ERank::JACK},
    {ESuit::DIAMONDS, ERank::QUEEN},
    {ESuit::DIAMONDS, ERank::KING},
    {ESuit::DIAMONDS, ERank::ACE},

    // HEARTS
    {ESuit::HEARTS, ERank::TWO},
    {ESuit::HEARTS, ERank::THREE},
    {ESuit::HEARTS, ERank::FOUR},
    {ESuit::HEARTS, ERank::FIVE},
    {ESuit::HEARTS, ERank::SIX},
    {ESuit::HEARTS, ERank::SEVEN},
    {ESuit::HEARTS, ERank::EIGHT},
    {ESuit::HEARTS, ERank::NINE},
    {ESuit::HEARTS, ERank::TEN},
    {ESuit::HEARTS, ERank::JACK},
    {ESuit::HEARTS, ERank::QUEEN},
    {ESuit::HEARTS, ERank::KING},
    {ESuit::HEARTS, ERank::ACE},

    // SPADES
    {ESuit::SPADES, ERank::TWO},
    {ESuit::SPADES, ERank::THREE},
    {ESuit::SPADES, ERank::FOUR},
    {ESuit::SPADES, ERank::FIVE},
    {ESuit::SPADES, ERank::SIX},
    {ESuit::SPADES, ERank::SEVEN},
    {ESuit::SPADES, ERank::EIGHT},
    {ESuit::SPADES, ERank::NINE},
    {ESuit::SPADES, ERank::TEN},
    {ESuit::SPADES, ERank::JACK},
    {ESuit::SPADES, ERank::QUEEN},
    {ESuit::SPADES, ERank::KING},
    {ESuit::SPADES, ERank::ACE},
}};
