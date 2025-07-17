#pragma once

#include "core/Types.hpp"

#include <string>
#include <string_view>

class Player {
public:
    Player(std::string name, Coins_t stack) noexcept;
    std::string_view GetName() const noexcept;
    Coins_t GetStack() const noexcept;
    void SetStack(Coins_t stack) noexcept;
    void IncreaseStack(Coins_t amount) noexcept;
    void DecreaseStack(Coins_t amount) noexcept;

private:
    std::string name_;
    Coins_t stack_{0.0};
};
