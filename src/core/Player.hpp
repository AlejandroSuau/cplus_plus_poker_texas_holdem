#pragma once

#include "core/Card.hpp"
#include "core/PlayerHand.hpp"

#include <string>
#include <array>

class Player {
public:
    Player(std::string name) noexcept;
    PlayerHand& GetHand() noexcept;

private:
    std::string name_;
    PlayerHand hand_;
};
