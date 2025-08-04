#pragma once

#include "core/Types.hpp"
#include "core/Card.hpp"

#include <unordered_set>

class Pot {
public:
    Pot(Coins_t amt) : amount(amt) {}
    
    Coins_t amount{0.0};
    std::unordered_set<std::size_t> players {};
};

class ITable {
public:
    using CommunityCards_t = std::vector<Card>;
    using Pots_t = std::vector<Pot>;

    virtual ~ITable() = default;

    virtual void SetBlindSmall(Coins_t cost) noexcept = 0;
    virtual void SetBlindBig(Coins_t cost) noexcept = 0;
    [[nodiscard]] virtual Coins_t GetBlindSmall() const noexcept = 0;
    [[nodiscard]] virtual Coins_t GetBlindBig() const noexcept = 0;
    
    virtual void IncreasePot(Coins_t amount) noexcept = 0;
    [[nodiscard]] virtual Coins_t CollectPot() noexcept = 0;
    [[nodiscard]] virtual Coins_t GetPot() const noexcept = 0;

    virtual void AddCommunityCard(Card card) noexcept = 0;
    [[nodiscard]] virtual const CommunityCards_t& GetCommunityCards() const noexcept = 0;

    virtual void ResetPots() = 0;
    [[nodiscard]] virtual const Pots_t& GetPots() const noexcept = 0;

    virtual void ExtractFromPot(Coins_t amount) = 0;
    virtual void ContributeToPot(std::size_t player_idx, Coins_t amount) = 0;
    virtual void RemovePlayerFromCurrentPot(std::size_t player_idx) = 0;
    virtual void AddPlayerToPot(std::size_t player_idx, std::size_t pot_idx) = 0;
    [[nodiscard]] virtual Pot& AddPot(Coins_t amount) = 0;
    [[nodiscard]] virtual Pot& GetPot(std::size_t pot_idx) = 0;
};
