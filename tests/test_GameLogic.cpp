#include <gtest/gtest.h>

#include <phevaluator/phevaluator.h>

#include "Config.hpp"

#include "core/Types.hpp"
#include "core/Card.hpp"
#include "core/Deck.hpp"
#include "core/Player.hpp"

#include "game_logic/GameLogic.hpp"

#include "utils/Translator.hpp"
#include "utils/Logger.hpp"

#include <memory>

class GameLogicTest : public ::testing::Test {
protected:
    std::unique_ptr<Deck> deck_;
    std::unique_ptr<Table> table_;
    Players players_;
    std::unique_ptr<GameLogic> logic_;

    void SetUp() override {
        deck_ = std::make_unique<Deck>(kCardDeck);
        table_ = std::make_unique<Table>(0.05, 0.10);
        players_ = {{
            {"P0", 10.0}, {"P1", 10.0}, {"P2", 10.0},
            {"P3", 10.0}, {"P4", 10.0}, {"P5", 10.0}}};
        logic_ = std::make_unique<GameLogic>(*deck_, *table_, players_);
    }

    void TearDown() override {

    }

    void StartHand() { logic_->StartHand(); }
    void Bet(Coins_t amount) { logic_->CurrentPlayerMakesAction(EPlayerAction::BET, amount); }
    void Raise(Coins_t amount) { logic_->CurrentPlayerMakesAction(EPlayerAction::RAISE, amount); }
    void Check() { logic_->CurrentPlayerMakesAction(EPlayerAction::CHECK); }
    void Call() { logic_->CurrentPlayerMakesAction(EPlayerAction::CALL); }
    void Fold() { logic_->CurrentPlayerMakesAction(EPlayerAction::FOLD); }
};

TEST_F(GameLogicTest, StartHand) {
    for (auto& player : players_) {
        EXPECT_TRUE(player.GetHand().IsEmpty());
    }

    StartHand();

    EXPECT_EQ(logic_->GetState(), EState::PREFLOP);

    for (auto& player : players_) {
        EXPECT_TRUE(player.GetHand().IsFull());
    }

    EXPECT_EQ(logic_->GetDealerIndex(), 0);
    EXPECT_EQ(logic_->GetCurrentPlayerIndex(), 3);
}

TEST_F(GameLogicTest, CompleteStateBettingPreFlop) {
    StartHand();

    Bet(0.3);   // P3
    Call();     // P4
    Raise(1.5); // P5
    Fold();     // P0
    Call();     // P1
    Call();     // P2
    Call();     // P3
    Call();     // P4; Making a higher bet than his previous 0.3

    EXPECT_TRUE(players_[0].IsFold());
    for (std::size_t i = 1; i < players_.size(); ++i) {
        EXPECT_EQ(players_[i].GetCoins(), 8.5);
    }

    EXPECT_EQ(table_->GetPot(), 1.5 * 5);
    EXPECT_EQ(logic_->GetState(), EState::PREFLOP);
    EXPECT_TRUE(logic_->IsBettingRoundComplete());

    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), EState::FLOP);
}

TEST_F(GameLogicTest, CompleteStateBettingFlop) {
    StartHand();

    // PREFLOP
    Bet(0.3); // P3
    Fold();   // P4
    Fold();   // P5
    Fold();   // P0
    Call();   // P1
    Call();   // P2
    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), EState::FLOP);
    EXPECT_EQ(table_->GetCommunityCards().size(), 3);
    EXPECT_EQ(logic_->GetCurrentPlayerIndex(), 1);

    // FLOP
    Check();  // P1
    Bet(0.5); // P2
    Call();   // P3
    Call();   // P1
    logic_->AdvanceState();
    
    EXPECT_EQ(logic_->GetState(), EState::TURN);
}

TEST_F(GameLogicTest, CompleteStateBettingTurn) {
    StartHand();

    // PREFLOP
    Bet(0.5);   // P3
    Call();     // P4
    Call();     // P5
    Call();     // P0
    Call();     // P1
    Call();     // P2
    logic_->AdvanceState();

    // FLOP
    Fold();     // P0
    Fold();     // P1
    Fold();     // P2
    Bet(0.45);  // P3
    Call();     // P4
    Call();     // P5
    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), EState::TURN);
    EXPECT_EQ(table_->GetCommunityCards().size(), 4);

    // TURN
    Bet(0.45);  // P3
    Call();     // P4
    Call();     // P5
    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), EState::RIVER);
    EXPECT_EQ(table_->GetCommunityCards().size(), 5);
}

TEST_F(GameLogicTest, CompleteStateBettingRiver) {
    StartHand();

    // PREFLOP
    Bet(0.5);   // P3 
    Fold();     // P4
    Fold();     // P5
    Fold();     // P0
    Fold();     // P1
    Call();     // P2
    logic_->AdvanceState();

    // FLOP
    Bet(0.3);     // P2
    Call();       // P3
    logic_->AdvanceState();

    // TURN
    Bet(0.3);     // P2
    Call();       // P3
    logic_->AdvanceState();

    // RIVER
    Bet(0.3);     // P2
    Call();       // P3
    // Set p1 card.
    // Set p2 card.
    // Set table cards.
    
    logic_->AdvanceState();
    EXPECT_EQ(logic_->GetState(), EState::SHOWDOWN);
    
    // Check winner.

    // Swap Cards, check winner p1.
    // check pot given
    // check pot empty
}

TEST_F(GameLogicTest, FinishHand) {
    StartHand();

    const auto p2_coins = players_[2].GetCoins();

    // PREFLOP
    Bet(0.3);   // P3 +0.3
    Fold();     // P4 -
    Fold();     // P5 -
    Fold();     // P0 -
    Fold();     // P1 +0.05
    Call();     // P2 +0.3
    logic_->AdvanceState();

    // FLOP
    Bet(0.3);     // P2 +0.3
    Call();       // P3 +0.3
    logic_->AdvanceState();

    // TURN
    Bet(0.3);     // P2 +0.3
    Call();       // P3 +0.3
    logic_->AdvanceState();

    // RIVER
    Bet(0.3);     // P2 +0.3
    Fold();       // P3

    EXPECT_FALSE(logic_->GetWinner().has_value());

    logic_->AdvanceState();
    
    const auto pot_amount = table_->GetPot();

    // SHOWDOWN state; We have a winner.
    EXPECT_EQ(logic_->GetState(), EState::SHOWDOWN);
    EXPECT_TRUE(logic_->GetWinner().has_value());
    
    const Coins_t expected_pot = (0.3 + 0.05 + 0.3) + (0.6) + (0.6) + 0.3;
    auto winner = logic_->GetWinner();
    EXPECT_EQ(winner->pot_amount_, pot_amount);
    EXPECT_EQ(winner->player->GetName(), players_[2].GetName().data());
    EXPECT_NEAR(pot_amount, expected_pot, 0.001);

    // Hand Finished and pot collected.
    logic_->AdvanceState();
    EXPECT_EQ(logic_->GetState(), EState::HAND_FINISHED);
    EXPECT_EQ(table_->GetPot(), 0.0);

    const Coins_t all_hand_bets = 0.3 * 4;
    const Coins_t expected_player_coins = p2_coins - all_hand_bets + pot_amount;
    Logger::Info("coins {}, bets {}, pot {}", p2_coins, all_hand_bets, pot_amount);
    EXPECT_NEAR(winner->player->GetCoins(), expected_player_coins, 0.001);
}
