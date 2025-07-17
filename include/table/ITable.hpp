#pragma once

#include "core/Types.hpp"
#include "core/Card.hpp"

#include <vector>

class ITable {
public:
    using CommunityCards_t = std::vector<Card>;

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
};
