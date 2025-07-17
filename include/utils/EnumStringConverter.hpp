#pragma once

#include "core/Card.hpp"
#include "core/Player.hpp"

#include <string_view>

namespace EnumString {

[[nodiscard]] inline constexpr std::string_view ToString(EPlayerAction action) noexcept {
    switch(action) {
        case EPlayerAction::BET:   return "Bet";
        case EPlayerAction::CHECK: return "Check";
        case EPlayerAction::CALL:  return "Call";
        case EPlayerAction::FOLD:  return "Fold";
        case EPlayerAction::RAISE: return "Raise";
        default:                   return "Unknown Action";
    }
}

[[nodiscard]] inline constexpr std::string_view ToString(ERank rank) noexcept {
    switch (rank) {
        case ERank::TWO:   return "2";
        case ERank::THREE: return "3";
        case ERank::FOUR:  return "4";
        case ERank::FIVE:  return "5";
        case ERank::SIX:   return "6";
        case ERank::SEVEN: return "7";
        case ERank::EIGHT: return "8";
        case ERank::NINE:  return "9";
        case ERank::TEN:   return "10";
        case ERank::JACK:  return "J";
        case ERank::QUEEN: return "Q";
        case ERank::KING:  return "K";
        case ERank::ACE:   return "A";
        default:           return "Unknown Rank";
    }
}

[[nodiscard]] inline constexpr std::string_view ToString(ESuit suit) noexcept {
    switch (suit) {
        case ESuit::HEARTS:   return "Hearts";
        case ESuit::DIAMONDS: return "Diamonds";
        case ESuit::CLUBS:    return "Clubs";
        case ESuit::SPADES:   return "Spades";
        default:              return "Unknown Suit";
    }
}

};
