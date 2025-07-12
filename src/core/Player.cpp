#include "core/Player.hpp"

#include <cassert>

Player::Player(std::string name, Coins_t coins) noexcept
    : name_(name), hand_(), coins_(coins), current_bet_(0), is_fold_(false) {}

PlayerHand& Player::GetHand() noexcept {
    return hand_;
}

Coins_t Player::GetCurrentBet() const noexcept {
    return current_bet_;
}

void Player::SetCurrentBet(Coins_t bet) noexcept {
    current_bet_ = bet;
}

void Player::SetCoins(Coins_t coins) noexcept {
    coins_ = coins;
}

Coins_t Player::GetCoins() noexcept {
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
