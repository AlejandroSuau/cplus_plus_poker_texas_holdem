#pragma once

#include "table/ITable.hpp"
#include "table/PlayerList.hpp"

#include "core/Types.hpp"
#include "core/Deck.hpp"
#include "core/Card.hpp"

// TODO: ALL-IN + Side pots

class Table : public ITable {
public:
    Table() noexcept = default;
    Table(Coins_t blind_small, Coins_t blind_big) noexcept;

    void SetBlindSmall(Coins_t cost) noexcept override;
    void SetBlindBig(Coins_t cost) noexcept override;
    [[nodiscard]] Coins_t GetBlindSmall() const noexcept override;
    [[nodiscard]] Coins_t GetBlindBig() const noexcept override;
    
     void IncreasePot(Coins_t amount) noexcept override; // remove
    [[nodiscard]] Coins_t CollectPot() noexcept override; // remove
    [[nodiscard]] Coins_t GetPot() const noexcept override; // remove

     void AddCommunityCard(Card card) noexcept override;
    [[nodiscard]] const CommunityCards_t& GetCommunityCards() const noexcept override;

    void ResetPots() override;
    [[nodiscard]] const Pots_t& GetPots() const noexcept override;
    
    void ExtractFromPot(Coins_t amount) override;
    void ContributeToPot(std::size_t player_idx, Coins_t amount) override;
    void RemovePlayerFromCurrentPot(std::size_t player_idx) override;
    void AddPlayerToPot(std::size_t player_idx, std::size_t pot_idx) override;
    [[nodiscard]] Pot& AddPot(Coins_t amount) override;
    [[nodiscard]] Pot& GetPot(std::size_t pot_idx) override;

private:
    Coins_t pot_{0.0}; // remove
    Coins_t blind_big_{0.0};
    Coins_t blind_small_{0.0};
    CommunityCards_t community_cards_;
    Pots_t pots_;
    std::size_t current_pot_idx_{0};
};
