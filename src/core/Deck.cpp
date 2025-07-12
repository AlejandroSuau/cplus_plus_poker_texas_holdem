#include "core/Deck.hpp"

#include <algorithm>
#include <random>
#include <ranges>
#include <cassert>
#include <chrono>

#include "utils/Logger.hpp"

// TODO: this move moves each card?
//       Is ranges shuffle the best?

Deck::Deck(Deck::DeckCards_t cards)  noexcept
    : cards_(cards), next_card_index_(0) {}
    
void Deck::Shuffle() noexcept {
    Logger::Info("Shuffling the deck ...");
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::random_device rd;
    std::mt19937 rng(rd());
    std::ranges::shuffle(cards_, rng);
    next_card_index_ = 0;
}

Card Deck::Draw() noexcept {
    if (next_card_index_ >= cards_.size()) {
        assert(false && "Trying to draw more cards than available!");
        return cards_[0];
    }
    Logger::Info("Drawing card: {}", cards_[next_card_index_].ToString());
    return cards_[next_card_index_++];
}

const Deck::DeckCards_t& Deck::GetCards() const noexcept {
    return cards_;
}