#include "core/Deck.hpp"

Deck::Deck(Deck::DeckCards_t cards, IRandomProvider& rng)  noexcept
    : cards_(cards), rng_(rng), next_card_index_(0) {}
    
void Deck::Shuffle() noexcept {
    rng_.Shuffle(cards_);
    next_card_index_ = 0;
}

std::optional<Card> Deck::Draw() noexcept {
    if (next_card_index_ >= cards_.size()) return std::nullopt;

    return cards_[next_card_index_++];
}

const Deck::DeckCards_t& Deck::GetCards() const noexcept {
    return cards_;
}