#pragma once

#include <phevaluator/phevaluator.h>

#include "core/Types.hpp"
#include "core/IDeck.hpp"
#include "core/Player.hpp"


#include "table/PlayerList.hpp"
#include "table/PlayerSession.hpp"
#include "table/ITable.hpp"

#include "utils/random/IRandomProvider.hpp"

#include <optional>

struct Seat {
    std::optional<Player> player {std::nullopt};
    PlayerSession session;
};

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
    static const std::size_t kMaxPlayers = 10;
    GameLogic(IDeck& deck, ITable& table, PlayerList& player_list);

    void StartHand();
    void ProcessPlayerAction(const Action& action);
    void AdvanceState();
    bool IsBettingRoundComplete() const;

    ELogicState GetState() const noexcept;
    std::optional<Winner> GetWinner() const noexcept;
    std::size_t GetDealerIndex() const noexcept;
    std::size_t GetCurrentPlayerIndex() const noexcept;

private:
    IDeck& deck_;
    ITable& table_;
    PlayerList& player_list_;

    ELogicState state_ {ELogicState::NONE};
    bool round_finished_{false};

    std::size_t dealer_index_{0};
    std::size_t index_blind_small_{0};
    std::size_t index_blind_big_{0};
    std::size_t current_player_index_{0};

    Coins_t highest_bet_{0.0};
    Coins_t last_raise_{0.0};

    std::optional<Winner> winner_{std::nullopt};

    void AdvanceTurn();
    void DealFlop();
    void DealTurn();
    void DealRiver();
    void HandleShowdown();
    void FinishHand();

    void ResetBets();
    
    std::size_t FindBestPlayer() const;
    void PayToPot(PlayerList::Seat& seat, Coins_t amount);
    void DrawCommunityCards(std::size_t quantity = 1);
};
