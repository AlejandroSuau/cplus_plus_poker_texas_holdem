#pragma once

#include "core/Card.hpp"

#include <vector>
#include <optional>

class IDeck {
public:
    using DeckCards_t = std::vector<Card>;
    virtual ~IDeck() = default;

    virtual void Shuffle() noexcept = 0;
    [[nodiscard]] virtual std::optional<Card> Draw() noexcept = 0;
    [[nodiscard]] virtual const DeckCards_t& GetCards() const noexcept = 0;
};
