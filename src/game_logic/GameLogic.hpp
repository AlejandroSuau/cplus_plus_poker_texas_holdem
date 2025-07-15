#pragma once

#include <phevaluator/phevaluator.h>

#include "core/Types.hpp"
#include "core/Deck.hpp"
#include "core/Player.hpp"
#include "core/Table.hpp"

#include <optional>

enum class EState {
    NONE,
    PREFLOP,
    FLOP,
    TURN,
    RIVER,
    SHOWDOWN,
    HAND_FINISHED
};

// TODO: Action + Bet on struct.

using Players = std::vector<Player>;

class GameLogic {
public:
    struct Winner {
        Player* player;
        phevaluator::Rank rank_;
        Coins_t pot_amount_;
    };

    GameLogic(Deck& deck, Table& table, Players& players);

    void StartHand();
    void CurrentPlayerMakesAction(EPlayerAction action, Coins_t bet = 0);
    void AdvanceState(); // So we can capture the last info in the state before moving to the next.

    EState GetState() const;
    std::size_t GetCurrentPlayerIndex() const;
    std::size_t GetDealerIndex() const;
    Coins_t GetHighestBet() const;
    bool IsBettingRoundComplete() const;

    std::optional<Winner> GetWinner() const;

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
    std::optional<Winner> winner_{std::nullopt};
    
    std::size_t GetNextPlayerToIndex(std::size_t index) const;
    std::size_t GetNextNonFoldPlayerToIndex(std::size_t index) const; // Non-fold one.

    void AdvanceTurn();

    void EnteringStateFlop();
    void EnteringStateTurn();
    void EnteringStateRiver();
    void EnteringStateShowdown();
    void EnteringStateHandFinished();

    std::size_t BestRankFromPlayerTableCards() const;
    void PrintAllPlayersRanks() const;
};
