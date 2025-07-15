#include "core/Card.hpp"

#include "utils/EnumStringConverter.hpp"

#include <cassert>
#include <sstream>

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
    std::ostringstream oss;
    oss << "Card {"
        << EnumString::ToString(rank_) << "-"
        << EnumString::ToString(suit_) << "}";
    return oss.str();
}