#pragma once

#include "table/ITable.hpp"
#include "core/Types.hpp"
#include "table/PlayerList.hpp"
#include "core/Deck.hpp"

// TODO: ALL-IN + Side pots

class Table : public ITable {
public:
    Table() noexcept = default;
    Table(Coins_t blind_small, Coins_t blind_big) noexcept;

    void SetBlindSmall(Coins_t cost) noexcept override;
    void SetBlindBig(Coins_t cost) noexcept override;
    [[nodiscard]] Coins_t GetBlindSmall() const noexcept override;
    [[nodiscard]] Coins_t GetBlindBig() const noexcept override;
    
     void IncreasePot(Coins_t amount) noexcept override;
    [[nodiscard]] Coins_t CollectPot() noexcept override;
    [[nodiscard]] Coins_t GetPot() const noexcept override;

     void AddCommunityCard(Card card) noexcept override;
    [[nodiscard]] const CommunityCards_t& GetCommunityCards() const noexcept override;

private:
    Coins_t pot_{0.0};
    Coins_t blind_big_{0.0};
    Coins_t blind_small_{0.0};
    CommunityCards_t community_cards_;
};
