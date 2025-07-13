#pragma once

#include "core/Types.hpp"
#include "core/Deck.hpp"
#include "core/Table.hpp"

enum class EState {
    NONE,
    PREFLOP,
    FLOP,
    TURN,
    RIVER,
    SHOWDOWN
};

// TODO: Action + Bet on struct.

using Players = std::vector<Player>;

class GameLogic {
public:
    GameLogic(Deck& deck, Table& table, Players& players);

    void StartHand();
    void CurrentPlayerMakesAction(EPlayerAction action, Coins_t bet = 0);
    void AdvanceState(); // So we can capture the last info in the state before moving to the next.

    EState GetState() const;
    std::size_t GetCurrentPlayerIndex() const;
    std::size_t GetDealerIndex() const;
    Coins_t GetHighestBet() const;
    bool IsBettingRoundComplete() const;

private:
    Deck& deck_;
    Table& table_;
    Players& players_;

    EState state_ {EState::NONE};
    bool state_did_finish_ {false};
    std::size_t dealer_index_;
    std::size_t index_blind_small_ {0};
    std::size_t index_blind_big_ {0};
    std::size_t current_player_index_ {0};
    Coins_t highest_bet_ {0.0};
    Coins_t last_raise_amount_ {0.0};

    std::size_t GetNextPlayerToIndex(std::size_t index) const;

    void AdvanceTurn();

    void EnteringStateFlop();
    void EnteringStateTurn();
    void EnteringStateRiver();
    void EnteringStateShowdown();
};
