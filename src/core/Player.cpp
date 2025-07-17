#include "core/Player.hpp"

#include <cassert>

Player::Player(std::string name, Coins_t coins) noexcept
    : name_(name), hand_(), coins_(coins), last_bet_(0.0), is_fold_(false) {}

std::string_view Player::GetName() const noexcept {
    return name_;
}

PlayerHand& Player::GetHand() noexcept {
    return hand_;
}

const PlayerHand& Player::GetHand() const noexcept {
    return hand_;
}

Coins_t Player::GetAlreadyPaid() const noexcept {
    return already_paid_;
}

void Player::SetAlreadyPaid(Coins_t amount) noexcept {
    already_paid_ = amount;
}

Coins_t Player::GetLastBet() const noexcept {
    return last_bet_;
}

void Player::SetLastBet(Coins_t bet) noexcept {
    last_bet_ = bet;
}

void Player::SetCoins(Coins_t coins) noexcept {
    coins_ = coins;
}

Coins_t Player::GetCoins() const noexcept {
    return coins_;
}

void Player::SetFold(bool is_fold) noexcept {
    is_fold_ = is_fold;
}

bool Player::IsFold() const noexcept {
    return is_fold_;
}

void Player::IncreaseCoins(Coins_t amount) noexcept {
    coins_ += amount;
}

void Player::DecreaseCoins(Coins_t amount) noexcept {
    assert((coins_ - amount) > 0 && "Player coins cannot be lower than 0");
    coins_ -= amount;
}
