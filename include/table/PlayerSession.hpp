#pragma once

#include <phevaluator/phevaluator.h>

#include "core/Card.hpp"
#include "core/Types.hpp"

#include <array>

// TODO: Create a EHandState: all_in, fold, playing.

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
    bool IsAllIn() const noexcept;
    void SetAllIn(bool all_in) noexcept;
    void SetLastBet(Coins_t bet) noexcept;
    void SetRank(phevaluator::Rank rank) noexcept;
    phevaluator::Rank GetRank() const noexcept;

private:
    Hand_t hand_;
    std::size_t cards_count_;
    bool is_fold_;
    bool is_all_in_;
    Coins_t last_bet_;
    phevaluator::Rank rank_;
};
