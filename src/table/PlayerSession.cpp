#include "table/PlayerSession.hpp"


PlayerSession::PlayerSession() noexcept {
    NewHand();
}

void PlayerSession::NewHand() noexcept {
    cards_count_ = 0;
    is_fold_ = false;
    is_all_in_ = false;
    last_bet_ = 0.0;
    rank_ = 0;
}

void PlayerSession::SetRank(phevaluator::Rank rank) noexcept {
    rank_ = rank;
}

phevaluator::Rank PlayerSession::GetRank() const noexcept {
    return rank_;
}


bool PlayerSession::AddCard(const Card& card) noexcept {
    if (cards_count_ == hand_.size()) return false;
    
    hand_[cards_count_++] = card;
    return true;
}

void PlayerSession::ClearHand() noexcept {
    cards_count_ = 0;
}

const PlayerSession::Hand_t& PlayerSession::GetHand() const noexcept {
    return hand_;
}

bool PlayerSession::IsFold() const noexcept {
    return is_fold_;
}

void PlayerSession::SetFold(bool fold) noexcept {
    is_fold_ = fold;
}

Coins_t PlayerSession::GetLastBet() const noexcept {
    return last_bet_;
}

void PlayerSession::SetLastBet(Coins_t bet) noexcept {
    last_bet_ = bet;
}

bool PlayerSession::IsAllIn() const noexcept {
    return is_all_in_;
}
void PlayerSession::SetAllIn(bool all_in) noexcept {
    is_all_in_ = all_in;
}
