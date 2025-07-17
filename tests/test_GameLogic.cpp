#include <gtest/gtest.h>

#include <phevaluator/phevaluator.h>

#include "Config.hpp"

#include "core/Types.hpp"
#include "core/Card.hpp"
#include "core/Player.hpp"
#include "table/ITable.hpp"
#include "table/Table.hpp"

#include "game_logic/GameLogic.hpp"

#include "utils/Translator.hpp"
#include "utils/Logger.hpp"

#include "mocks/MockDeck.hpp"
#include "mocks/MockTable.hpp"

#include <memory>
#include <stdexcept>
#include <string>

using ::testing::Return;
using ::testing::_;

Players MakePlayers(std::size_t n, Coins_t chips = 100.0) {
    Players players;
    for (std::size_t i = 0; i < n; ++i) {
        players.emplace_back("P" + std::to_string(i), chips);
    }
    return players;
}

class GameLogicTest : public ::testing::Test {
protected:
    // NiceMock for allowing not to EXPECT_CALL of all process functions from Dekc,
    std::unique_ptr<testing::NiceMock<MockDeck>> mock_deck_;
    std::unique_ptr<testing::NiceMock<MockTable>> mock_table_;
    std::unique_ptr<IDeck> deck_;
    std::unique_ptr<ITable> table_;
    Players players_;
    std::unique_ptr<GameLogic> logic_;

    void SetUp() override {
        mock_deck_ = std::make_unique<testing::NiceMock<MockDeck>>();
        mock_table_ = std::make_unique<testing::NiceMock<MockTable>>();
        deck_ = std::make_unique<Deck>(kCardDeck);
        table_ = std::make_unique<Table>(2.0, 4.0);
        players_ = MakePlayers(6);
        logic_ = std::make_unique<GameLogic>(*mock_deck_, *mock_table_, players_);

        std::optional<Card> fake_card = Card{ESuit::CLUBS, ERank::ACE};
        EXPECT_CALL(*mock_deck_, Draw()).WillRepeatedly(Return(fake_card));

        EXPECT_CALL(*mock_table_, GetBlindSmall()).WillRepeatedly(Return(2.0));
        EXPECT_CALL(*mock_table_, GetBlindBig()).WillRepeatedly(Return(4.0));
    }
};

// === === GENERAL BEHAVIOUR AND BASIC RULES === ===

TEST_F(GameLogicTest, DealerRotatesCorrectly) {
    const size_t prev_dealer = logic_->GetDealerIndex();
    logic_->StartHand();
    EXPECT_EQ(logic_->GetDealerIndex(), (prev_dealer + 1) % players_.size());
    logic_->StartHand();
    EXPECT_EQ(logic_->GetDealerIndex(), (prev_dealer + 2) % players_.size());
}

// === === PLAYER ACTIONS: BET, RAISE, CALL, CHECK, FOLD === ===

TEST_F(GameLogicTest, CannotBetLessThanBigBlind) {
    logic_->StartHand();
    EXPECT_THROW(logic_->ProcessPlayerAction({EPlayerAction::BET, 1.0}), std::runtime_error);
}

TEST_F(GameLogicTest, CannotBetWhenBettingIsOpen) {
    logic_->StartHand();
    logic_->ProcessPlayerAction({EPlayerAction::BET, 4.0});
    EXPECT_THROW(logic_->ProcessPlayerAction({EPlayerAction::BET, 6.0}), std::runtime_error);
}

TEST_F(GameLogicTest, CannotCheckWhenBetExists) {
    logic_->StartHand();
    logic_->ProcessPlayerAction({EPlayerAction::BET, 4.0});
    EXPECT_THROW(logic_->ProcessPlayerAction({EPlayerAction::CHECK}), std::runtime_error);
}

TEST_F(GameLogicTest, RaiseMustBeAtLeastLastRaise) {
    logic_->StartHand();
    logic_->ProcessPlayerAction({EPlayerAction::BET, 4.0});
    logic_->ProcessPlayerAction({EPlayerAction::RAISE, 8.0});
    EXPECT_THROW(logic_->ProcessPlayerAction({EPlayerAction::RAISE, 9.0}), std::runtime_error);
    EXPECT_THROW(logic_->ProcessPlayerAction({EPlayerAction::RAISE, 11.0}), std::runtime_error);
    logic_->ProcessPlayerAction({EPlayerAction::RAISE, 12.0});
}

// === === PREFLOP -> SHOWDOWN === ===

TEST_F(GameLogicTest, CannotAdvanceStateIfNotFinished) {
    logic_->StartHand();
    EXPECT_THROW(logic_->AdvanceState(), std::runtime_error);
}

// === === EDGE CASES === ===

TEST_F(GameLogicTest, PlayerWinsByAllFoldAndPotIsCorrect) {
    auto players = MakePlayers(3);
    logic_ = std::make_unique<GameLogic>(*deck_, *table_, players);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::FOLD});
    logic_->ProcessPlayerAction({EPlayerAction::FOLD});

    auto winner = logic_->GetWinner();
    ASSERT_TRUE(winner);
    ASSERT_EQ(winner->player_index, 2);
    ASSERT_EQ(winner->pot_amount, 6.0);

    EXPECT_EQ(players[0].GetCoins(), 100.0);
    EXPECT_EQ(players[1].GetCoins(), 98.0);
    EXPECT_EQ(players[2].GetCoins(), 102.0);
}

TEST_F(GameLogicTest, AllPlayersButOneFoldHandFinishes) {
    logic_->StartHand();
    for (size_t i = 0; i < players_.size() - 1; ++i) {
        logic_->ProcessPlayerAction({EPlayerAction::FOLD});
    }

    auto winner = logic_->GetWinner();
    EXPECT_TRUE(winner);
    EXPECT_EQ(logic_->GetState(), EState::HAND_FINISHED);
}

TEST_F(GameLogicTest, SinglePlayerLeftEndsHand) {
    auto players = MakePlayers(2);
    logic_ = std::make_unique<GameLogic>(*deck_, *table_, players);
    logic_->StartHand();
    logic_->ProcessPlayerAction({EPlayerAction::FOLD});
    
    auto winner = logic_->GetWinner();
    EXPECT_EQ(logic_->CountActivePlayers(), 1);
    EXPECT_TRUE(winner);
    EXPECT_EQ(logic_->GetState(), EState::HAND_FINISHED);
}

TEST_F(GameLogicTest, AllPlayersCheckAndTryToAdvanceState) {
    logic_->StartHand();
    for (size_t i = 0; i < players_.size(); ++i) {
        logic_->ProcessPlayerAction({EPlayerAction::CHECK});
    }

    EXPECT_FALSE(logic_->IsBettingRoundComplete());
    EXPECT_THROW(logic_->AdvanceState(), std::runtime_error);
    EXPECT_EQ(logic_->GetState(), EState::PREFLOP);
}

// === === TESTS DE INTEGRIDAD Y CONSISTENCIA === ===

TEST_F(GameLogicTest, CommunityCardsAreDealtCorrectly) {
    logic_ = std::make_unique<GameLogic>(*deck_, *table_, players_);
    
    logic_->StartHand();

    // preflop - flop - turn - river
    for (int s = 0; s < 4; ++s) {
        for (size_t i = 0; i < players_.size(); ++i) {
            if (i == 0) {
                logic_->ProcessPlayerAction({EPlayerAction::BET, 4.0});
            } else {
                logic_->ProcessPlayerAction({EPlayerAction::CALL});
            }
        }
        logic_->AdvanceState();
    }

    EXPECT_EQ(table_->GetCommunityCards().size(), 5);
    EXPECT_EQ(logic_->GetState(), EState::SHOWDOWN);
}

// === === ALL-IN, SIDE POT, TIE === ===

TEST_F(GameLogicTest, PlayerAllInScenario) {
    // Simulate all-in
    // ...
}

TEST_F(GameLogicTest, SplitPotScenario) {
    // Simulate perfect tie: half pot for each player.
    // ...
}

// === === RESTART TESTS === ===

TEST_F(GameLogicTest, HandCanRestartAfterFinish) {
    logic_->StartHand();
    for (size_t i = 0; i < players_.size() - 1; ++i) {
        logic_->ProcessPlayerAction({EPlayerAction::FOLD});
    }
    
    EXPECT_TRUE(logic_->GetWinner());
    EXPECT_EQ(logic_->GetState(), EState::HAND_FINISHED);

    logic_->StartHand();
    EXPECT_EQ(logic_->GetState(), EState::PREFLOP);
}
