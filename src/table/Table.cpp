#include "table/Table.hpp"

Table::Table(Coins_t blind_small, Coins_t blind_big) noexcept
    : pot_(0.0), blind_small_(blind_small), blind_big_(blind_big) {
    pots_.emplace_back();
}

void Table::SetBlindSmall(Coins_t cost) noexcept {
    blind_small_ = cost;
}

void Table::SetBlindBig(Coins_t cost) noexcept {
    blind_big_ = cost;
}

Coins_t Table::CollectPot() noexcept {
    auto pot = pot_;
    pot_ = 0;
    return pot;
}

void Table::IncreasePot(Coins_t value) noexcept {
    pot_ += value;
    pots_[current_pot_].amount += value;
}

void Table::AddCommunityCard(Card card) noexcept {
    community_cards_.push_back(card);
}

Coins_t Table::GetPot() const noexcept {
    return pot_;
}

Coins_t Table::GetBlindSmall() const noexcept {
    return blind_small_;
}

Coins_t Table::GetBlindBig() const noexcept {
    return blind_big_;
}

const ITable::CommunityCards_t& Table::GetCommunityCards() const noexcept {
    return community_cards_;
}

void Table::ResetPots() {
    pots_.clear();
    pots_.emplace_back();
    current_pot_ = 0;
}

void Table::AddPot() {
    pots_.emplace_back();
    ++current_pot_;
}

void Table::AddPlayerToPot(std::size_t player_idx) {
    pots_[current_pot_].players.push_back(player_idx);
}

const ITable::Pots_t& Table::GetPots() const noexcept {
    return pots_;
}