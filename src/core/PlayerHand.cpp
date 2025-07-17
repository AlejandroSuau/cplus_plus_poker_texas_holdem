#include "core/PlayerHand.hpp"

#include <cassert>

PlayerHand::PlayerHand() noexcept
    : count_(0) {}

bool PlayerHand::AddCard(Card card) noexcept {
    if (IsFull()) return false;

    cards_[count_++] = card;
    return true;
}

void PlayerHand::Clear() noexcept {
    count_ = 0;
}

const PlayerHand::Cards_t& PlayerHand::GetCards() const noexcept {
    return cards_;
}

bool PlayerHand::IsEmpty() const noexcept {
    return (count_ == 0);
}

bool PlayerHand::IsFull() const noexcept {
    return (count_ == cards_.size());
}