#pragma once

#include "core/Types.hpp"

#include <string>

class Card {
public:
    Card() noexcept;

    Card(ECardSuit suit, ECardRank rank) noexcept;
    ECardSuit GetSuit() const noexcept;
    ECardRank GetRank() const noexcept;

    bool operator==(const Card& other) const noexcept;

    std::string ToString() const noexcept;

private:
    ECardSuit suit_;
    ECardRank rank_;
};
