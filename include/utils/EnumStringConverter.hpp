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

[[nodiscard]] inline constexpr std::string_view ToString(ECardRank rank) noexcept {
    switch (rank) {
        case ECardRank::TWO:   return "2";
        case ECardRank::THREE: return "3";
        case ECardRank::FOUR:  return "4";
        case ECardRank::FIVE:  return "5";
        case ECardRank::SIX:   return "6";
        case ECardRank::SEVEN: return "7";
        case ECardRank::EIGHT: return "8";
        case ECardRank::NINE:  return "9";
        case ECardRank::TEN:   return "10";
        case ECardRank::JACK:  return "J";
        case ECardRank::QUEEN: return "Q";
        case ECardRank::KING:  return "K";
        case ECardRank::ACE:   return "A";
        default:               return "Unknown Rank";
    }
}

[[nodiscard]] inline constexpr std::string_view ToString(ECardSuit suit) noexcept {
    switch (suit) {
        case ECardSuit::HEARTS:   return "Hearts";
        case ECardSuit::DIAMONDS: return "Diamonds";
        case ECardSuit::CLUBS:    return "Clubs";
        case ECardSuit::SPADES:   return "Spades";
        default:                  return "Unknown Suit";
    }
}

};
