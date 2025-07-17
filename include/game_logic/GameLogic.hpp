#pragma once

#include <phevaluator/phevaluator.h>

#include "core/Types.hpp"
#include "core/IDeck.hpp"
#include "table/ITable.hpp"
#include "core/Player.hpp"

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

using Players = std::vector<Player>;

struct Action {
    EPlayerAction action;
    Coins_t amount {0.0};
};

struct Winner {
    std::size_t player_index;
    phevaluator::Rank rank;
    Coins_t pot_amount;
};

class GameLogic {
public:
    GameLogic(IDeck& deck, ITable& table, Players& players);

    void StartHand();
    void ProcessPlayerAction(const Action& action);
    void AdvanceState();
    bool IsBettingRoundComplete() const;

    EState GetState() const noexcept;
    std::optional<Winner> GetWinner() const noexcept;
    std::size_t GetDealerIndex() const noexcept;
    std::size_t GetCurrentPlayerIndex() const noexcept;
    const Players& GetPlayers() const noexcept;

    std::size_t FindOnlyActivePlayer() const;
    std::size_t CountActivePlayers() const;

private:
    IDeck& deck_;
    ITable& table_;
    Players& players_;

    EState state_ {EState::NONE};
    bool round_finished_{false};

    std::size_t dealer_index_{0};
    std::size_t index_blind_small_{0};
    std::size_t index_blind_big_{0};
    std::size_t current_player_index_{0};

    Coins_t highest_bet_{0.0};
    Coins_t last_raise_{0.0};

    std::optional<Winner> winner_{std::nullopt};
    
    std::size_t NextPlayerIndex(std::size_t index) const;
    std::size_t NextActivePlayerIndex(std::size_t index) const;

    void AdvanceTurn();
    void DealFlop();
    void DealTurn();
    void DealRiver();
    void HandleShowdown();
    void FinishHand();

    void ResetBets();
    
    std::size_t FindBestPlayer() const;
    void PayToPot(Player& player, Coins_t amount);
    void DrawCommunityCards(std::size_t quantity = 1);
};
