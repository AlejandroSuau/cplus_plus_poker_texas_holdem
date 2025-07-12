#pragma once

#include "core/Types.hpp"
#include "core/PlayerList.hpp"
#include "core/Deck.hpp"

// TODO: ALL-IN + Side pots

class Table {
public:
    Table();
    Table(Coins_t blind_small, Coins_t blind_big);

    void SetBlindSmall(Coins_t cost);
    void SetBlindBig(Coins_t cost);
    void AddCommunityCard(Card card);
    void IncreasePot(Coins_t value);
    Coins_t ConsumePot();
    
    Coins_t GetPot() const;
    Coins_t GetBlindSmall() const;
    Coins_t GetBlindBig() const;

private:
    Coins_t pot_;
    Coins_t blind_big_;
    Coins_t blind_small_;
    std::vector<Card> community_cards_;
};
