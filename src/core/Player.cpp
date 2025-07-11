#include "core/Player.hpp"

Player::Player(std::string name) noexcept
    : name_(name) {} 

PlayerHand& Player::GetHand() noexcept {
    return hand_;
}
