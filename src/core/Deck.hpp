#pragma once

#include "core/IDeck.hpp"

#include <vector>
// TODO: Array or Vector? Which is performance better and why? And for sorting?
//       Shared ptr? Objects? Why? Copies are better? At least theye're for testing.

class Deck : public IDeck {
public:
    using DeckCards_t = std::vector<Card>;
    Deck(DeckCards_t cards) noexcept;

    void Shuffle() noexcept override;
    [[nodiscard]] Card Draw() noexcept override;
    [[nodiscard]] const DeckCards_t& GetCards() const noexcept override;

private:
    DeckCards_t cards_;
    std::size_t next_card_index_;
};