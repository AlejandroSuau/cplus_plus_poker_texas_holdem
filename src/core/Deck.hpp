#pragma once

#include "core/Card.hpp"

#include <vector>
// TODO: Array or Vector? Which is performance better and why? And for sorting?
//       Shared ptr? Objects? Why? Copies are better? At least theye're for testing.

class Deck {
public:
    using DeckCards_t = std::vector<Card>;
    Deck(DeckCards_t cards) noexcept;

    void Shuffle() noexcept;
    Card Draw() noexcept;

    const DeckCards_t& GetCards() const noexcept;

private:
    DeckCards_t cards_;
    std::size_t next_card_index_;
};