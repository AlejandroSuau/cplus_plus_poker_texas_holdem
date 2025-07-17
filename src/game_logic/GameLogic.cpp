#include "game_logic/GameLogic.hpp"

#include <phevaluator/phevaluator.h>

#include "utils/Translator.hpp"
#include "utils/Logger.hpp"

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <ranges>

GameLogic::GameLogic(IDeck& deck, ITable& table, PlayerList& player_list) 
    : deck_(deck)
    , table_(table)
    , player_list_(player_list) {
    dealer_index_ = *player_list_.LastOccupiedSeat();
}

void GameLogic::StartHand() {
    dealer_index_ = *player_list_.NextOccupiedSeat(dealer_index_);
    index_blind_small_ = *player_list_.NextOccupiedSeat(dealer_index_);
    index_blind_big_ = *player_list_.NextOccupiedSeat(index_blind_small_);

    deck_.Shuffle();

    for (auto& seat_idx : player_list_.GetActiveSeatIndices()) {
        auto& session = player_list_.GetSession(seat_idx);
        session.NewHand();
        for (std::size_t i = 0; i < 2; ++i) {
            auto maybe_card = deck_.Draw();
            if (!maybe_card) {
                throw std::runtime_error("No available draw cards for players");
            }
            session.AddCard(*maybe_card);
        }
    }

    const Coins_t sb = table_.GetBlindSmall();
    const Coins_t bb = table_.GetBlindBig();

    PayToPot(player_list_.GetSeat(index_blind_small_), sb);
    PayToPot(player_list_.GetSeat(index_blind_big_), bb);

    highest_bet_ = 0.0;
    last_raise_ = 0.0;
    current_player_index_ = *player_list_.NextOccupiedSeat(index_blind_big_);

    state_ = ELogicState::PREFLOP;
    round_finished_ = false;
    winner_.reset();
}

void GameLogic::ProcessPlayerAction(const Action& action) {
    auto& player_seat = player_list_.GetSeat(current_player_index_);
    auto& player_session = player_seat.session;
    bool has_to_pay = false;

    switch(action.action) {
        case EPlayerAction::BET:
            if (highest_bet_ != 0.0) { throw std::runtime_error("Cannot BET when betting is already open."); }
            if (action.amount < table_.GetBlindBig()) { throw std::runtime_error("Bet below minimum big blind."); }
            
            player_session.SetLastBet(action.amount);
            highest_bet_ = action.amount;
            has_to_pay = true;
            break;
        case EPlayerAction::CALL:
            player_session.SetLastBet(highest_bet_);
            has_to_pay = true;
        break;
        case EPlayerAction::CHECK:
            if (highest_bet_ != 0.0) { throw std::runtime_error("Cannot CHECK when bet exists"); }
        break;
        case EPlayerAction::FOLD:
            player_session.SetFold(true);
            player_session.ClearHand();
            if (player_list_.CountActiveSeats() == 1) {
                const auto winner_index = *player_list_.FindFirstActiveSeat();
                winner_ = Winner{winner_index, 0, table_.GetPot()};
                FinishHand();
            }
        break;
        case EPlayerAction::RAISE:
            if (highest_bet_ == 0.0) { throw std::runtime_error("Cannot RAISE before a bet exists"); }
            if (action.amount < highest_bet_ + last_raise_) { throw std::runtime_error("Raise too small"); }
            
            last_raise_ = action.amount - highest_bet_;
            highest_bet_ = action.amount;
            player_session.SetLastBet(action.amount);
            has_to_pay = true;
        break;
    }

    if (has_to_pay) {
        PayToPot(player_seat, action.amount - player_session.GetAlreadyPaid());
    }

    AdvanceTurn();
}

void GameLogic::PayToPot(PlayerList::Seat& seat, Coins_t amount) {
    if (amount <= 0) return;

    auto& player = *seat.player;
    player.DecreaseStack(amount);
    
    auto& player_session = seat.session;
    player_session.SetAlreadyPaid(player_session.GetAlreadyPaid() + amount);
    table_.IncreasePot(amount);
}

void GameLogic::AdvanceTurn() {
    current_player_index_ = *player_list_.NextActiveSeat(current_player_index_);
    round_finished_ = IsBettingRoundComplete();
}

void GameLogic::AdvanceState() {
    if (!round_finished_) {
        throw std::runtime_error("Round not finished yet");
    }

    switch(state_) {
        case ELogicState::PREFLOP:   DealFlop(); break;
        case ELogicState::FLOP:      DealTurn(); break;
        case ELogicState::TURN:      DealRiver(); break;
        case ELogicState::RIVER:     HandleShowdown(); break;
        case ELogicState::SHOWDOWN:  FinishHand(); break;
        default: break;
    }
}

void GameLogic::DealFlop() {
    state_ = ELogicState::FLOP;
    ResetBets();
    DrawCommunityCards(3);
    current_player_index_ = *player_list_.NextActiveSeat(index_blind_big_);
    round_finished_ = false;
}

void GameLogic::DealTurn() {
    state_ = ELogicState::TURN;
    ResetBets();
    DrawCommunityCards(1);
    current_player_index_ = *player_list_.NextActiveSeat(index_blind_big_);
    round_finished_ = false;
}

void GameLogic::DealRiver() {
    state_ = ELogicState::RIVER;
    ResetBets();
    DrawCommunityCards(1);
    current_player_index_ = *player_list_.NextActiveSeat(index_blind_big_);
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
    state_ = ELogicState::SHOWDOWN;
    const auto winner_index = FindBestPlayer();
    const auto& winner_session = player_list_.GetSession(winner_index);

    const auto rank = Translator::RankFromPlayerTableCards(
        winner_session.GetHand(),
        table_.GetCommunityCards()
    );

    winner_ = Winner{winner_index, rank, table_.GetPot()};
    round_finished_ = true;
}

void GameLogic::FinishHand() {
    if (!winner_) throw std::runtime_error("No winner found!");

    auto& winner_player = *player_list_.GetPlayer(winner_->player_index);
    const auto pot = table_.CollectPot();
    winner_player.IncreaseStack(pot);
    state_ = ELogicState::HAND_FINISHED;
}

void GameLogic::ResetBets() {
    highest_bet_ = 0.0;
    last_raise_ = 0.0;
    for (auto& p : player_list_) {
        p.session.SetLastBet(0.0);
    }
}

bool GameLogic::IsBettingRoundComplete() const {
    return std::all_of(player_list_.begin(), player_list_.end(), [&](const auto& seat) {
        return (!seat.player || seat.session.IsFold() || (highest_bet_ > 0.0 && seat.session.GetLastBet() == highest_bet_));
    });
}

std::size_t GameLogic::FindBestPlayer() const {
    std::size_t best_index = 0;
    phevaluator::Rank best_rank;

    for (auto seat_idx : player_list_.GetActiveSeatIndices()) {
        const auto rank = Translator::RankFromPlayerTableCards(
            player_list_.GetSession(seat_idx).GetHand(),
            table_.GetCommunityCards()
        );
        if (best_rank == 0 || rank > best_rank) {
            best_rank = rank;
            best_index = seat_idx;
        }
    }

    if (best_rank == 0) {
        throw std::runtime_error("No valid best rank found!");
    }

    return best_index;
}

ELogicState GameLogic::GetState() const noexcept {
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
