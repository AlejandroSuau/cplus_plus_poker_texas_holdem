#include "game_logic/GameLogic.hpp"

#include <cassert>

GameLogic::GameLogic(Deck& deck, Table& table, Players& players) 
    : deck_(deck), table_(table), players_(players), state_(EState::NONE) {}

void GameLogic::StartHand() {
    deck_.Shuffle();
    
    bet_ = 0;
    dealer_index_ = 0;
    index_blind_small_ = GetNextPlayerToIndex(dealer_index_);
    index_blind_big_ = GetNextPlayerToIndex(index_blind_small_);

    // Player pays before start.
    const auto small_blind_amount = table_.GetBlindSmall();
    players_[index_blind_small_].DecreaseCoins(small_blind_amount);
    table_.IncreasePot(small_blind_amount);

    const auto big_blind_amount = table_.GetBlindBig();
    players_[index_blind_big_].DecreaseCoins(big_blind_amount);
    table_.IncreasePot(big_blind_amount);

    // Give private cards to players.
    for (auto& player : players_) {
        auto& player_hand = player.GetHand();
        player_hand.Clear();
        for (std::size_t i = 0; i < 2; ++i) {
            player_hand.AddCard(deck_.Draw());
        }
    }

    state_ = EState::PREFLOP;
    current_player_index_ = GetNextPlayerToIndex(index_blind_big_); // UTG, under the gun.
}

void GameLogic::PlayerMakesAction(EPlayerAction action, Coins_t bet) {
    // apuesta mínima valor ciega grande.
    auto& player = players_[current_player_index_];
    // FOLD / CALL / RAISE (si ya hay una apuesta en la mesa)
    switch(action) {
        case EPlayerAction::BET:
            // Cuando nadie haya apostado (independientemente de las mínima y máxima)
            assert(bet >= table_.GetBlindBig());
            table_.IncreasePot(bet);
            player.SetCurrentBet(bet);
            player.DecreaseCoins(bet);
            bet_ = bet;
        break;
        case EPlayerAction::CALL:
            assert(bet == bet_ && "Play bet should be equals to the biggest");

            table_.IncreasePot(bet);
            player.SetCurrentBet(bet);
            player.DecreaseCoins(bet);
        break;
        case EPlayerAction::CHECK:
            assert(state_ != EState::PREFLOP && "Player can not check in prefold state"); // You can only if you're big blind and no one bet before.
            assert(bet_ == 0 && "Player can only check if there is not an available bet");
        break;
        case EPlayerAction::FOLD:
            player.SetFold(true);
            player.GetHand().Clear();
            // Te retiras. Se eliminan tus cartas. Y pierdes la apuesta mínima / máxima si te toca.
            // Si nadie hubiese apostado, en vez de fold, puedes pasar.
        break;
        case EPlayerAction::RAISE:
            assert(bet >= bet_ + last_raise_value_);
            table_.IncreasePot(bet);
            player.DecreaseCoins(bet);
            bet_ = bet;
            last_raise_value_ = bet;
        break;
        default:
            assert(false && "Unexpected player action");
        break;
    }

    AdvanceTurn();
}

std::size_t GameLogic::GetNextPlayerToIndex(std::size_t index) const {
    return ((index + 1) % players_.size());
}

void GameLogic::AdvanceTurn() {
    current_player_index_ = GetNextPlayerToIndex(current_player_index_);
    // Si nadie más puede actuar, cambiamos de estado.
}

void GameLogic::AdvanceState() {
    switch(state_) {
        case EState::PREFLOP:
            state_ = EState::FLOP;
        break;
        case EState::FLOP:
            state_ = EState::TURN;
        break;
        case EState::TURN:
            state_ = EState::RIVER;
        break;
        case EState::RIVER:
            state_ = EState::SHOWDOWN;
        break;
    }
}

std::size_t GameLogic::GetCurrentPlayerIndex() const {
    return current_player_index_;
}

EState GameLogic::GetState() const {
    return state_;
}