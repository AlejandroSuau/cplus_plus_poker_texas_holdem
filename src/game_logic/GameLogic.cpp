#include "game_logic/GameLogic.hpp"

#include <phevaluator/phevaluator.h>

#include "utils/Translator.hpp"
#include "utils/Logger.hpp"

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <ranges>

GameLogic::GameLogic(IDeck& deck, ITable& table, Players& players) 
    : deck_(deck)
    , table_(table)
    , players_(players) {
    dealer_index_ = players_.size() - 1;
}

void GameLogic::StartHand() {
    dealer_index_ = NextPlayerIndex(dealer_index_);
    index_blind_small_ = NextPlayerIndex(dealer_index_);
    index_blind_big_ = NextPlayerIndex(index_blind_small_);

    deck_.Shuffle();

    for (auto& p : players_) {
        auto& hand = p.GetHand();
        hand.Clear();
        p.SetFold(false);
        p.SetLastBet(0.0);
        p.SetAlreadyPaid(0.0);
        for (std::size_t i = 0; i < 2; ++i) {
            auto maybe_card = deck_.Draw();
            if (!maybe_card) {
                throw std::runtime_error("No available draw cards for players");
            }
            hand.AddCard(*maybe_card);
        }
    }

    const Coins_t sb = table_.GetBlindSmall();
    const Coins_t bb = table_.GetBlindBig();

    PayToPot(players_[index_blind_small_], sb);
    PayToPot(players_[index_blind_big_], bb);

    highest_bet_ = 0.0;
    last_raise_ = 0.0;
    current_player_index_ = NextPlayerIndex(index_blind_big_);

    state_ = EState::PREFLOP;
    round_finished_ = false;
    winner_.reset();
}

void GameLogic::ProcessPlayerAction(const Action& action) {
    auto& player = players_[current_player_index_];
    bool has_to_pay = false;

    switch(action.action) {
        case EPlayerAction::BET:
            if (highest_bet_ != 0.0) { throw std::runtime_error("Cannot BET when betting is already open."); }
            if (action.amount < table_.GetBlindBig()) { throw std::runtime_error("Bet below minimum big blind."); }
            
            player.SetLastBet(action.amount);
            highest_bet_ = action.amount;
            has_to_pay = true;
            break;
        case EPlayerAction::CALL:
            player.SetLastBet(highest_bet_);
            has_to_pay = true;
        break;
        case EPlayerAction::CHECK:
            if (highest_bet_ != 0.0) { throw std::runtime_error("Cannot CHECK when bet exists"); }
        break;
        case EPlayerAction::FOLD:
            player.SetFold(true);
            player.GetHand().Clear();
            if (CountActivePlayers() == 1) {
                const auto winner_index = FindOnlyActivePlayer();
                winner_ = Winner{winner_index, 0, table_.GetPot()};
                FinishHand();
            }
        break;
        case EPlayerAction::RAISE:
            if (highest_bet_ == 0.0) { throw std::runtime_error("Cannot RAISE before a bet exists"); }
            if (action.amount < highest_bet_ + last_raise_) { throw std::runtime_error("Raise too small"); }
            
            last_raise_ = action.amount - highest_bet_;
            highest_bet_ = action.amount;
            player.SetLastBet(action.amount);
            has_to_pay = true;
        break;
    }

    if (has_to_pay) {
        PayToPot(player, action.amount - player.GetAlreadyPaid());
    }

    AdvanceTurn();
}

void GameLogic::PayToPot(Player& player, Coins_t amount) {
    if (amount <= 0) return;

    player.DecreaseCoins(amount);
    player.SetAlreadyPaid(player.GetAlreadyPaid() + amount);
    table_.IncreasePot(amount);
}

void GameLogic::AdvanceTurn() {
    current_player_index_ = NextActivePlayerIndex(current_player_index_);
    round_finished_ = IsBettingRoundComplete();
}

std::size_t GameLogic::FindOnlyActivePlayer() const {
    for (std::size_t i = 0; i < players_.size(); ++i) {
        if (!players_[i].IsFold()) return i;
    }

    throw std::runtime_error("No active player found");
}

std::size_t GameLogic::CountActivePlayers() const {
    return std::ranges::count_if(players_, [](const Player& p) { return !p.IsFold(); });
}

void GameLogic::AdvanceState() {
    if (!round_finished_) {
        throw std::runtime_error("Round not finished yet");
    }

    switch(state_) {
        case EState::PREFLOP:   DealFlop(); break;
        case EState::FLOP:      DealTurn(); break;
        case EState::TURN:      DealRiver(); break;
        case EState::RIVER:     HandleShowdown(); break;
        case EState::SHOWDOWN:  FinishHand(); break;
        default: break;
    }
}

std::size_t GameLogic::NextPlayerIndex(std::size_t index) const {
    return (index + 1) % players_.size();
}

std::size_t GameLogic::NextActivePlayerIndex(std::size_t index) const {
    std::size_t i = index;
    do {
        i = NextPlayerIndex(i);
    } while (players_[i].IsFold());
    
    return i;
}

void GameLogic::DealFlop() {
    state_ = EState::FLOP;
    ResetBets();
    DrawCommunityCards(3);
    current_player_index_ = index_blind_big_;
    while (players_[current_player_index_].IsFold()) {
        current_player_index_ = NextPlayerIndex(current_player_index_);
    }

    round_finished_ = false;
}

void GameLogic::DealTurn() {
    state_ = EState::TURN;
    ResetBets();
    DrawCommunityCards(1);
    current_player_index_ = index_blind_big_;
    while (players_[current_player_index_].IsFold()) {
        current_player_index_ = NextPlayerIndex(current_player_index_);
    }

    round_finished_ = false;
}

void GameLogic::DealRiver() {
    state_ = EState::RIVER;
    ResetBets();
    DrawCommunityCards(1);
    current_player_index_ = index_blind_small_;
    while (players_[current_player_index_].IsFold())
        current_player_index_ = NextPlayerIndex(current_player_index_);

    round_finished_ = false;
}

void GameLogic::DrawCommunityCards(std::size_t quantity) {
    for (std::size_t i = 0; i < quantity; ++i) {
        auto maybe_card = deck_.Draw();
        if (!maybe_card) {
            throw std::runtime_error("No available draw cards for community");
        }
        table_.AddCommunityCard(*maybe_card);
    }
}

void GameLogic::HandleShowdown() {
    state_ = EState::SHOWDOWN;
    const auto winner_index = FindBestPlayer();
    const auto& winner_player = players_[winner_index];

    const auto rank = Translator::RankFromPlayerTableCards(
        winner_player.GetHand().GetCards(),
        table_.GetCommunityCards()
    );

    winner_ = Winner{winner_index, rank, table_.GetPot()};
    round_finished_ = true;
}

void GameLogic::FinishHand() {
    if (!winner_) throw std::runtime_error("No winner found!");

    auto& winner = players_[winner_->player_index];
    const auto pot = table_.CollectPot();
    winner.IncreaseCoins(pot);
    state_ = EState::HAND_FINISHED;
}

void GameLogic::ResetBets() {
    highest_bet_ = 0.0;
    last_raise_ = 0.0;
    for (auto& p : players_) {
        p.SetLastBet(0.0);
    }
}

bool GameLogic::IsBettingRoundComplete() const {
    return std::all_of(players_.cbegin(), players_.cend(), [&](const auto& p) {
        return (p.IsFold() || (highest_bet_ > 0.0 && p.GetLastBet() == highest_bet_));
    });
}

std::size_t GameLogic::FindBestPlayer() const {
    std::size_t best_index = 0;
    phevaluator::Rank best_rank;

    for (std::size_t i = 0; i < players_.size(); ++i) {
        if (players_[i].IsFold()) continue;

        const auto rank = Translator::RankFromPlayerTableCards(
            players_[i].GetHand().GetCards(),
            table_.GetCommunityCards()
        );
        if (best_rank == 0 || rank > best_rank) {
            best_rank = rank;
            best_index = i;    
        }
    }

    if (best_rank == 0) {
        throw std::runtime_error("No valid best rank found!");
    }

    return best_index;
}

EState GameLogic::GetState() const noexcept {
    return state_;
}

std::optional<Winner> GameLogic::GetWinner() const noexcept {
    return winner_;
}

std::size_t GameLogic::GetDealerIndex() const noexcept {
    return dealer_index_;
}

std::size_t GameLogic::GetCurrentPlayerIndex() const noexcept {
    return current_player_index_;
}

const Players& GameLogic::GetPlayers() const noexcept {
    return players_;
}
