#include "game_logic/GameLogic.hpp"

#include <cassert>
#include <algorithm>

#include "utils/Logger.hpp"

GameLogic::GameLogic(Deck& deck, Table& table, Players& players) 
    : deck_(deck), table_(table), players_(players), state_(EState::NONE), dealer_index_(players_.size() - 1) {}

void GameLogic::StartHand() {
    // 1. Assignate dealer and blinds.
    dealer_index_ = GetNextPlayerToIndex(dealer_index_);
    index_blind_small_ = GetNextPlayerToIndex(dealer_index_);
    index_blind_big_ = GetNextPlayerToIndex(index_blind_small_);

    // 2. Pay blinds (You don't pay ... what if you increase bet)
    const auto small_blind_amount = table_.GetBlindSmall();
    players_[index_blind_small_].SetLastBet(small_blind_amount);

    const auto big_blind_amount = table_.GetBlindBig();
    players_[index_blind_big_].SetLastBet(big_blind_amount);

    // 3. Shuffle Deck.
    deck_.Shuffle();

    // 4. Draw 2 private cards each player.
    for (auto& player : players_) {
        auto& player_hand = player.GetHand();
        player_hand.Clear();
        for (std::size_t i = 0; i < 2; ++i) {
            player_hand.AddCard(deck_.Draw());
        }
    }

    // 5. Init vars.
    highest_bet_ = big_blind_amount;
    last_raise_amount_ = big_blind_amount;
    current_player_index_ = GetNextPlayerToIndex(index_blind_big_);
    state_ = EState::PREFLOP;
    state_did_finish_ = false;
}

void GameLogic::CurrentPlayerMakesAction(EPlayerAction action, Coins_t bet) {
    auto& player = players_[current_player_index_];
    switch(action) {
        case EPlayerAction::BET:
            // Cuando nadie haya apostado (independientemente de las mínima y máxima)
            assert(bet >= table_.GetBlindBig());
            player.SetLastBet(bet);
            highest_bet_ = bet;
        break;
        case EPlayerAction::CALL:
            // assert(bet == highest_bet_ && "Play bet should be equals to the biggest");
            player.SetLastBet(highest_bet_);
        break;
        case EPlayerAction::CHECK:
            assert(state_ != EState::PREFLOP && "Player can not check in prefold state"); // You can only if you're big blind and no one bet before.
            assert(highest_bet_ == 0 && "Player can only check if there is not an available bet");
        break;
        case EPlayerAction::FOLD: {
            const auto player_last_bet = player.GetLastBet();
            table_.IncreasePot(player_last_bet);
            player.DecreaseCoins(player_last_bet);
            player.SetFold(true);
            player.GetHand().Clear();
            player.SetLastBet(0.0);
        }
        break;
        case EPlayerAction::RAISE:
            assert(bet >= highest_bet_ + last_raise_amount_);
            player.SetLastBet(bet);
            highest_bet_ = bet;
            last_raise_amount_ = bet - highest_bet_;
        break;
        default:
            assert(false && "Unexpected player action");
            return;
    }

    AdvanceTurn();
}

std::size_t GameLogic::GetNextPlayerToIndex(std::size_t index) const {
    return ((index + 1) % players_.size());
}

void GameLogic::AdvanceTurn() {
    current_player_index_ = GetNextPlayerToIndex(current_player_index_);
    
    if (IsBettingRoundComplete()) {
        for (auto& player : players_) {
            if (player.IsFold()) continue;

            const auto player_last_bet = player.GetLastBet();
            table_.IncreasePot(player_last_bet);
            player.DecreaseCoins(player_last_bet);
        }
        state_did_finish_ = true;
    }
}

void GameLogic::AdvanceState() {
    if (!state_did_finish_) {
        assert(false && "You can't advance state because it didn't finish yet!");
        return;
    }

    switch(state_) {
        case EState::PREFLOP:
            EnteringStateFlop();
        break;
        case EState::FLOP:
            EnteringStateTurn();
        break;
        case EState::TURN:
            EnteringStateRiver();
        break;
        case EState::RIVER:
            EnteringStateShowdown();
        break;
    }
}

void GameLogic::EnteringStateFlop() {
    state_ = EState::FLOP;
    
    // 1. Player turn in small position index.
    current_player_index_ = index_blind_small_;
    while (players_[current_player_index_].IsFold()) {
        current_player_index_ = GetNextPlayerToIndex(current_player_index_);
    }

    // 2. Add 3 community cards.
    for (std::size_t i = 0; i < 3; ++i) {
        table_.AddCommunityCard(deck_.Draw());
    }
}

void GameLogic::EnteringStateTurn() {
    state_ = EState::TURN;
    current_player_index_ = index_blind_small_;
    // 1. Actua la ciega pequeña.
    // 2. Draw community card.
}

void GameLogic::EnteringStateRiver() {
    state_ = EState::RIVER;
    // 1. Actua la ciega pequeña.
    // 2. Draw community card.
}

void GameLogic::EnteringStateShowdown() {
    state_ = EState::SHOWDOWN;

    // 1. Show private cards.
    // 2. Evaluate hand.
    // 3. Give the prize.

}

bool GameLogic::IsBettingRoundComplete() const {
    return std::all_of(
        players_.cbegin(),
        players_.cend(),
        [&](const Player& p) { return (p.IsFold() || p.GetLastBet() == highest_bet_); }
    );
}

Coins_t GameLogic::GetHighestBet() const {
    return highest_bet_;
}

std::size_t GameLogic::GetCurrentPlayerIndex() const {
    return current_player_index_;
}

std::size_t GameLogic::GetDealerIndex() const {
    return dealer_index_;
}

EState GameLogic::GetState() const {
    return state_;
}