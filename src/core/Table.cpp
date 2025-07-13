#include "core/Table.hpp"

Table::Table()
    : pot_(0), blind_small_(0), blind_big_(0) {}

Table::Table(Coins_t blind_small, Coins_t blind_big)
    : pot_(0), blind_small_(blind_small), blind_big_(blind_big) {}

void Table::SetBlindSmall(Coins_t cost) {
    blind_small_ = cost;
}

void Table::SetBlindBig(Coins_t cost) {
    blind_big_ = cost;
}

Coins_t Table::ConsumePot() {
    auto pot = pot_;
    pot_ = 0;
    return pot;
}

void Table::IncreasePot(Coins_t value) {
    pot_ += value;
}

void Table::AddCommunityCard(Card card) {
    community_cards_.push_back(card);
}

Coins_t Table::GetPot() const {
    return pot_;
}

Coins_t Table::GetBlindSmall() const {
    return blind_small_;
}

Coins_t Table::GetBlindBig() const {
    return blind_big_;
}

const std::vector<Card>& Table::GetCommunityCards() const noexcept {
    return community_cards_;
}
