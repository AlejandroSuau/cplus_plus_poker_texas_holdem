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
    Coins_t GetCoins() const noexcept;
    Coins_t GetLastBet() const noexcept;
    Coins_t GetAlreadyPaid() const noexcept;
    std::string_view GetName() const noexcept;
    void SetCoins(Coins_t coins) noexcept;
    void SetAlreadyPaid(Coins_t amound) noexcept;
    void SetLastBet(Coins_t bet) noexcept;
    void SetFold(bool is_fold) noexcept;
    bool IsFold() const noexcept;

    void IncreaseCoins(Coins_t amount) noexcept;
    void DecreaseCoins(Coins_t amount) noexcept;

private:
    std::string name_;
    PlayerHand hand_;
    Coins_t coins_{0.0};
    Coins_t already_paid_{0.0};
    Coins_t last_bet_{0.0};
    bool is_fold_{false};
};
