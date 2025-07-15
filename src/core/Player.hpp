#pragma once

#include "core/Types.hpp"

#include "core/Card.hpp"
#include "core/PlayerHand.hpp"

#include <string>
#include <array>
#include <string_view>

enum class EPlayerAction {
    FOLD,
    CHECK,
    CALL,
    BET,
    RAISE
};

class Player {
public:
    Player(std::string name, Coins_t coins) noexcept;
    PlayerHand& GetHand() noexcept;
    const PlayerHand& GetHand() const noexcept;
    Coins_t GetCoins() noexcept;
    Coins_t GetLastBet() const noexcept;
    std::string_view GetName() const noexcept;
    void SetCoins(Coins_t coins) noexcept;
    void SetLastBet(Coins_t bet) noexcept;
    void SetFold(bool is_fold) noexcept;
    bool IsFold() const noexcept;

    void IncreaseCoins(Coins_t amount) noexcept;
    void DecreaseCoins(Coins_t amount) noexcept;

private:
    std::string name_;
    PlayerHand hand_;
    Coins_t coins_;
    Coins_t last_bet_;
    bool is_fold_;
};
