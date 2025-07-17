#include "table/PlayerSession.hpp"


PlayerSession::PlayerSession() noexcept {
    NewHand();
}

void PlayerSession::NewHand() noexcept {
    count_ = 0;
    is_fold_ = false;
    last_bet_ = 0.0;
    already_paid_ = 0.0;
}

bool PlayerSession::AddCard(const Card& card) noexcept {
    if (count_ == hand_.size()) return false;
    
    hand_[count_++] = card;
    return true;
}

void PlayerSession::ClearHand() noexcept {
    count_ = 0;
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

Coins_t PlayerSession::GetAlreadyPaid() const noexcept {
    return already_paid_;
}

void PlayerSession::SetAlreadyPaid(Coins_t paid) noexcept {
    already_paid_ = paid;
}
