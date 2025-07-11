#pragma once

#include "core/Card.hpp"

#include <array>

class PlayerHand {
public:
    using Cards_t = std::array<Card, 2>;
    PlayerHand() noexcept;

    void AddCard(Card card) noexcept;
    void Clear() noexcept;
    const Cards_t& GetCards() const noexcept;
    bool IsEmpty() const noexcept;
    bool IsFull() const noexcept;

private:
    Cards_t cards_;
    std::size_t count_;
};
