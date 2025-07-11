#include "core/Deck.hpp"

#include <algorithm>
#include <random>
#include <ranges>
#include <cassert>

// TODO: this move moves each card?
//       Is ranges shuffle the best?

Deck::Deck(Deck::DeckCards_t cards)  noexcept
    : cards_(std::move(cards)), next_card_index_(0) {}
    
void Deck::Shuffle() noexcept {
    auto rng = std::default_random_engine{};
    std::ranges::shuffle(cards_, rng);
    next_card_index_ = 0;
}

Card Deck::Draw() noexcept {
    if (next_card_index_ >= cards_.size()) {
        assert(false && "Trying to draw more cards than available!");
        return cards_[0];
    }
    return cards_[next_card_index_++];
}

const Deck::DeckCards_t& Deck::GetCards() const noexcept {
    return cards_;
}