#include "core/Card.hpp"

#include <cassert>

Card::Card() noexcept 
    : suit_(ESuit::SPADES), rank_(ERank::ACE) {}

Card::Card(ESuit suit, ERank rank) noexcept 
    : suit_(suit), rank_(rank) {}

ESuit Card::GetSuit() const noexcept {
    return suit_;
}

ERank Card::GetRank() const noexcept {
    return rank_;
}

bool Card::operator==(const Card& other) const noexcept {
    return (GetSuit() == other.GetSuit() &&
            GetRank() == other.GetRank());
}

std::string Card::ToString() const {
    std::string rank_str;
    switch (rank_) {
        case ERank::TWO:   rank_str = "2"; break;
        case ERank::THREE: rank_str = "3"; break;
        case ERank::FOUR:  rank_str = "4"; break;
        case ERank::FIVE:  rank_str = "5"; break;
        case ERank::SIX:   rank_str = "6"; break;
        case ERank::SEVEN: rank_str = "7"; break;
        case ERank::EIGHT: rank_str = "8"; break;
        case ERank::NINE:  rank_str = "9"; break;
        case ERank::TEN:   rank_str = "10"; break;
        case ERank::JACK:  rank_str = "J"; break;
        case ERank::QUEEN: rank_str = "Q"; break;
        case ERank::KING:  rank_str = "K"; break;
        case ERank::ACE:   rank_str = "A"; break;
        default:
            assert(false && "Unknown rank");
    }

    std::string suit_str;
    switch (suit_) {
        case ESuit::HEARTS:   suit_str = "Hearts"; break;
        case ESuit::DIAMONDS: suit_str = "Diamonds"; break;
        case ESuit::CLUBS:    suit_str = "Clubs"; break;
        case ESuit::SPADES:   suit_str = "Spades"; break;
        default:
            assert(false && "Unknown suit");
    }

    return "{Card: " + rank_str + " - " + suit_str + "}";
}