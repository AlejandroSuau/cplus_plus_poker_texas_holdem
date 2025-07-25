#pragma once

#include "core/IDeck.hpp"

#include "utils/random/StdRandomProvider.hpp"

#include <vector>
#include <optional>

class Deck : public IDeck {
public:
    using DeckCards_t = std::vector<Card>;
    Deck(DeckCards_t cards, IRandomProvider& rng) noexcept;

    void Shuffle() noexcept override;
    [[nodiscard]] std::optional<Card> Draw() noexcept override;
    [[nodiscard]] const DeckCards_t& GetCards() const noexcept override;

private:
    DeckCards_t cards_;
    IRandomProvider& rng_;
    std::size_t next_card_index_;
};