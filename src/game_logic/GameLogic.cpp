#include "game_logic/GameLogic.hpp"

#include <phevaluator/phevaluator.h>

#include "utils/Translator.hpp"
#include "utils/Logger.hpp"

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <ranges>

#include <set>

GameLogic::GameLogic(IDeck& deck, ITable& table, PlayerList& player_list) 
    : deck_(deck)
    , table_(table)
    , player_list_(player_list) {
    dealer_index_ = *player_list_.LastOccupiedSeat();
}

void GameLogic::StartHand() {
    table_.ResetPots();

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

    const auto sb = table_.GetBlindSmall();
    const auto bb = table_.GetBlindBig();

    player_list_.GetSession(index_blind_small_).SetLastBet(sb);
    player_list_.GetSession(index_blind_big_).SetLastBet(bb);
    PayToPot(index_blind_small_, sb);
    PayToPot(index_blind_big_, bb);

    highest_bet_ = bb;
    last_raise_ = 0.0;
    current_player_index_ = *player_list_.NextOccupiedSeat(index_blind_big_);

    state_ = ELogicState::PREFLOP;
    round_finished_ = false;
    did_someone_bet_ = false;
}

void GameLogic::PayToPot(std::size_t player_idx, Coins_t amount) {
    table_.ContributeToPot(player_idx, amount);
    
    auto& player = player_list_.GetPlayer(player_idx);
    player.SetStack(player.GetStack() - amount);

    const bool is_all_in = (player.GetStack() == 0.0);
    player_list_.GetSession(player_idx).SetAllIn(is_all_in);

    Logger::Debug("#### #### Player {} #### #### ", player_idx);
    Logger::Debug("# Betting: {}", amount);
    Logger::Debug("# Player Stack: {}", player.GetStack());
    Logger::Debug("#### #### #### #### #### ####");
}

void GameLogic::ProcessPlayerAction(const Action& action) {
    auto& player_seat = player_list_.GetSeat(current_player_index_);
    auto& player_session = player_seat.session;

    if (action.amount > 0.0) {
        // TODO: move player stack to session
        did_someone_bet_ = true; // we need this?
        PayToPot(current_player_index_, action.amount - player_session.GetLastBet());
        player_session.SetLastBet(action.amount);
        highest_bet_ = action.amount;
    } else {
        // fold / check
        if (action.action == EPlayerAction::FOLD) {
            table_.RemovePlayerFromCurrentPot(current_player_index_);
            player_session.ClearHand();
            player_session.SetFold(true);
            if (player_list_.CountActiveSeats() == 1) { // TODO: move this to method
                HandleShowdown();
                return;
            }
        }
    }

    AdvanceTurn();
}

void GameLogic::AdvanceTurn() {
    // TODO: Refactor project later.
    current_player_index_ = *player_list_.NextActiveSeat(current_player_index_);
    round_finished_ = IsBettingRoundComplete();
    
    if (round_finished_) {
        // TODO: Fix this.
        // TODO: If someone went all in.
            // 0 players has more chips to play. => HandleShowDonw()?
            // 1 player has more chips to play. => Fix that last bet. Subtract from pot that already putted in. 
            // N player has more chips to play. => Add 1 pot, keep playing.
        
        const auto active_players = player_list_.GetActiveSeatIndices();
        std::size_t count_all_in_players = player_list_.CountAllInPlayers();

        // If everyone is all-in. 
        //      Split in pots. (There can be pots of 1). Do not clear pots, just add new ones.
        // Elseif all - 1.
        //      Split in pots.
        // Else (all-N-1 or more).
        //      Keep playing.

        if (count_all_in_players >= active_players.size() - 1) {
            ComputePotsAmount();
            HandleShowdown();
        } else if (count_all_in_players == 0) {
           // Nothing?
        } else {
            // Fix overbet players.
            highest_bet_ = player_list_.GetMinLastBet();;
            for (const auto i : active_players) {
                auto& player = player_list_.GetPlayer(i);
                auto& session = player_list_.GetSession(i);
                const auto last_bet = session.GetLastBet();
                if (last_bet > highest_bet_) {
                    const Coins_t overbet = last_bet - highest_bet_;
                    session.SetLastBet(highest_bet_);
                    table_.ExtractFromPot(overbet);
                    player.IncreaseStack(overbet);
                }
            }

            // Add one pot. Keep playing there.
            table_.AddPot(0.0);
        } 
    }
}

void GameLogic::ComputePotsAmount() {
    // TODO: Fix this method.
    // Last pot is broken. Fix and continue.
    std::vector<Pot> pots_without_last;
    auto pots = table_.GetPots();
    if (pots.size() > 1) {
        pots_without_last = std::vector<Pot>(pots.begin(), pots.end() - 1);
    }

    table_.ResetPots();

    std::set<Coins_t> pot_amounts {};
    const auto active_player_indices = player_list_.GetActiveSeatIndices();
    for (const auto i : active_player_indices) {
        const auto& session = player_list_.GetSession(i);
        if (!session.IsAllIn()) continue;

        pot_amounts.emplace(session.GetLastBet());
    }

    for (const auto current_amount : pot_amounts) {
        auto& pot = table_.AddPot(current_amount);
        
        for (const auto i : active_player_indices) {
            const auto& player_session = player_list_.GetSession(i);
            if (player_session.GetLastBet() >= current_amount) {
                pot.players.insert(i);
            }
        }
    }
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
    current_player_index_ = *player_list_.NextActiveSeat(dealer_index_);
    round_finished_ = false;
}

void GameLogic::DealTurn() {
    state_ = ELogicState::TURN;
    ResetBets();
    DrawCommunityCards(1);
    current_player_index_ = *player_list_.NextActiveSeat(dealer_index_);
    round_finished_ = false;
}

void GameLogic::DealRiver() {
    state_ = ELogicState::RIVER;
    ResetBets();
    DrawCommunityCards(1);
    current_player_index_ = *player_list_.NextActiveSeat(dealer_index_);
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

    // On Showdown should be always 5 cards. Even if coming from a preflop all-in.
    const auto community_cards_count = table_.GetCommunityCards().size();
    if (community_cards_count < 5) {
        DrawCommunityCards(5 - community_cards_count);
    }

    ComputePlayersRank();
    round_finished_ = true;
}

void GameLogic::ComputePlayersRank() {
    auto& pots = table_.GetPots();
    for (auto& pot : pots) {
        for (auto& player_idx : pot.players) {
            const auto rank = Translator::RankFromPlayerTableCards(
                player_list_.GetSession(player_idx).GetHand(),
                table_.GetCommunityCards());
            
            player_list_.GetSession(player_idx).SetRank(rank);
        }
    }
}

void GameLogic::FinishHand() {
    // if (state_ !=) throw std::runtime_error("No winner found!");

    // TODO: Implement.
        // 1. Compare ranks and seek for tie.
        // 2. Give prizes. If there's a tie. Divide the amount between players.
        // 2. Reset pot.

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
        return (!seat.player || 
                seat.session.IsFold() ||
                seat.session.IsAllIn() ||
                (did_someone_bet_ && seat.session.GetLastBet() == highest_bet_));
    });
}

ELogicState GameLogic::GetState() const noexcept {
    return state_;
}

std::size_t GameLogic::GetDealerIndex() const noexcept {
    return dealer_index_;
}

std::size_t GameLogic::GetCurrentPlayerIndex() const noexcept {
    return current_player_index_;
}
