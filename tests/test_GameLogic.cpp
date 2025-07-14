#include <gtest/gtest.h>

#include <phevaluator/phevaluator.h>

#include "Config.hpp"

#include "core/Types.hpp"
#include "core/Card.hpp"
#include "core/Deck.hpp"
#include "core/Player.hpp"

#include "game_logic/GameLogic.hpp"

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

    phevaluator::Rank a = phevaluator::EvaluateCards("9c", "4c", "4s", "9d", "4h");
    phevaluator::Rank b = phevaluator::EvaluateCards("9c", "4c", "4s", "9d", "9h");

    ASSERT_EQ(a.value(), 292);
    ASSERT_EQ(b.value(), 236);
}

TEST_F(GameLogicTest, CompleteStateBettingRiver) {
}
