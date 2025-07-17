#include "core/Player.hpp"

#include <cassert>

Player::Player(std::string name, Coins_t stack) noexcept
    : name_(name), stack_(stack) {}

Coins_t Player::GetStack() const noexcept {
    return stack_;
}

void Player::SetStack(Coins_t stack) noexcept {
    stack_ = stack;
}

void Player::IncreaseStack(Coins_t amount) noexcept {
    stack_ += amount;
}

void Player::DecreaseStack(Coins_t amount) noexcept {
    stack_ -= amount;
}

std::string_view Player::GetName() const noexcept {
    return name_;
}
