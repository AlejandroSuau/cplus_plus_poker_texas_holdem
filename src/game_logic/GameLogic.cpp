#include "game_logic/GameLogic.hpp"

#include <phevaluator/phevaluator.h>

#include "utils/Translator.hpp"
#include "utils/Logger.hpp"

#include <cassert>
#include <algorithm>

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
    winner_ = std::nullopt;
}

void GameLogic::CurrentPlayerMakesAction(EPlayerAction action, Coins_t bet) {
    auto& player = players_[current_player_index_];
    // TODO: Validate action
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

std::size_t GameLogic::GetNextNonFoldPlayerToIndex(std::size_t index) const {
    // TODO: We assume there is at least 1 non-fold.
    std::size_t i = index;
    do {
        i = GetNextPlayerToIndex(i);
    } while(players_[i].IsFold());
    
    return i;
}

void GameLogic::AdvanceTurn() {
    current_player_index_ = GetNextNonFoldPlayerToIndex(current_player_index_);
    if (IsBettingRoundComplete()) {
        state_did_finish_ = true;
        for (auto& player : players_) {
            if (player.IsFold()) continue;

            const auto player_last_bet = player.GetLastBet();
            table_.IncreasePot(player_last_bet);
            player.DecreaseCoins(player_last_bet);
        }
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
        case EState::SHOWDOWN:
            EnteringStateHandFinished();
        break;
    }
}

void GameLogic::EnteringStateFlop() {
    // TODO: Reset players last bet?
    state_did_finish_ = false;
    state_ = EState::FLOP;
    highest_bet_ = 0.0;
    last_raise_amount_ = 0.0;
    for (auto& p : players_) {
        p.SetLastBet(0.0);
    }

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
    // TODO: Reset players last bet?
    state_did_finish_ = false;
    state_ = EState::TURN;
    current_player_index_ = index_blind_small_;
    for (auto& p : players_) {
        p.SetLastBet(0.0);
    }

    while (players_[current_player_index_].IsFold()) {
        current_player_index_ = GetNextPlayerToIndex(current_player_index_);
    }
    
    table_.AddCommunityCard(deck_.Draw());
}

void GameLogic::EnteringStateRiver() {
    state_did_finish_ = false;
    state_ = EState::RIVER;
    current_player_index_ = index_blind_small_;
    for (auto& p : players_) {
        p.SetLastBet(0.0);
    }
    while (players_[current_player_index_].IsFold()) {
        current_player_index_ = GetNextPlayerToIndex(current_player_index_);
    }
    
     table_.AddCommunityCard(deck_.Draw());
}

void GameLogic::EnteringStateShowdown() {
    state_ = EState::SHOWDOWN;

    // Check who wins
    auto best_player_index = BestRankFromPlayerTableCards();
    auto& pcards = players_[best_player_index].GetHand().GetCards();
    auto& tcards = table_.GetCommunityCards();
    const auto rank = Translator::RankFromPlayerTableCards(pcards, tcards);
    Logger::Info("... ... ... ...");
    Logger::Info("Player {} wins with {}", best_player_index, rank.describeCategory());
    Logger::Info("... ... ... ...");

    winner_ = Winner{&players_[best_player_index], rank, table_.GetPot()};
    state_did_finish_ = true;
}

void GameLogic::EnteringStateHandFinished() {
    assert(winner_->player && "No winner player set");
    state_ = EState::HAND_FINISHED;
    
    const Coins_t pot_amount = table_.CollectPot();
    winner_->player->IncreaseCoins(pot_amount);
    state_did_finish_ = true;
    // Restart Game?
}

std::size_t GameLogic::BestRankFromPlayerTableCards() const {
    std::size_t best_index = 0;
    phevaluator::Rank best_rank;
    for (std::size_t i = 0; i < players_.size(); ++i) {
        if (players_[i].IsFold()) continue;

        const auto rank = Translator::RankFromPlayerTableCards(
            players_[i].GetHand().GetCards(), table_.GetCommunityCards());
        if (best_rank == 0 || rank > best_rank) {
            best_index = i;
            best_rank = rank;
        }
    }

    assert(best_rank != 0 && "There must be a best rank");

    return best_index;
}

bool GameLogic::IsBettingRoundComplete() const {
    return std::all_of(
        players_.cbegin(),
        players_.cend(),
        [&](const Player& p) { return (p.IsFold() || p.GetLastBet() == highest_bet_); }
    );
}

void GameLogic::PrintAllPlayersRanks() const {
    for (std::size_t i = 0; i < players_.size(); ++i) {
        if (players_[i].IsFold()) continue;

        auto& pcards = players_[i].GetHand().GetCards();
        auto& tcards = table_.GetCommunityCards();
        const auto rank = Translator::RankFromPlayerTableCards(pcards, tcards);

        Logger::Info("... Cards ...");
        for (auto& c : pcards) { Logger::Info("{}", c.ToString()); }
        for (auto& c : tcards) { Logger::Info("{}", c.ToString()); }
        Logger::Info("RANK {}", rank.describeCategory());
        Logger::Info("... ... ... ...");
    }
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

std::optional<GameLogic::Winner> GameLogic::GetWinner() const {
    return winner_;
}