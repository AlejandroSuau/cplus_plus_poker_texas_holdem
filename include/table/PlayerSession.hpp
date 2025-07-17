#pragma once

#include "core/Card.hpp"
#include "core/Types.hpp"

#include <array>

class PlayerSession {
public:
    using Hand_t = std::array<Card, 2>;
    PlayerSession() noexcept;

    void NewHand() noexcept;
    bool AddCard(const Card& card) noexcept;
    void ClearHand() noexcept;
    
    const Hand_t& GetHand() const noexcept;
    bool IsFold() const noexcept;
    void SetFold(bool fold) noexcept;
    Coins_t GetLastBet() const noexcept;
    void SetLastBet(Coins_t bet) noexcept;
    Coins_t GetAlreadyPaid() const noexcept;
    void SetAlreadyPaid(Coins_t paid) noexcept;

private:
    Hand_t hand_;
    std::size_t count_;
    bool is_fold_;
    Coins_t last_bet_;
    Coins_t already_paid_;
};
