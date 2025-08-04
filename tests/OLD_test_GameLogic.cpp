#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Config.hpp"

#include "core/Card.hpp"
#include "core/Player.hpp"

#include "utils/random/IRandomProvider.hpp"
#include "utils/random/StdRandomProvider.hpp"

#include "table/ITable.hpp"
#include "table/Table.hpp"
#include "table/PlayerList.hpp"

#include "game_logic/GameLogic.hpp"

#include "mocks/MockDeck.hpp"
#include "mocks/MockTable.hpp"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
/*
// --- Fixture ---
class GameLogicTest : public ::testing::Test {
protected:
    std::unique_ptr<testing::NiceMock<MockDeck>> mock_deck_;
    std::unique_ptr<testing::NiceMock<MockTable>> mock_table_;
    PlayerList player_list_;
    std::unique_ptr<IRandomProvider> rng_;
    std::unique_ptr<GameLogic> logic_;

    void SetUp() override {
        mock_deck_ = std::make_unique<testing::NiceMock<MockDeck>>();
        mock_table_ = std::make_unique<testing::NiceMock<MockTable>>();
        rng_ = std::make_unique<StdRandomProvider>();

        // Fill seats 0, 1, 2 with players
        player_list_.SitPlayerAt(MakePlayer("A"), 0);
        player_list_.SitPlayerAt(MakePlayer("B"), 1);
        player_list_.SitPlayerAt(MakePlayer("C"), 2);

        // Always return the same card for simplicity in mocks
        std::optional<Card> fake_card = Card{ECardSuit::HEARTS, ECardRank::ACE};
        EXPECT_CALL(*mock_deck_, Draw()).WillRepeatedly(Return(fake_card));
        EXPECT_CALL(*mock_table_, GetBlindSmall()).WillRepeatedly(Return(2.0));
        EXPECT_CALL(*mock_table_, GetBlindBig()).WillRepeatedly(Return(4.0));

        logic_ = std::make_unique<GameLogic>(*mock_deck_, *mock_table_, player_list_);
    }

    static Player MakePlayer(const std::string& name, Coins_t chips = 100.0) {
        return Player(name, chips);
    }
};

TEST_F(GameLogicTest, DealerRotatesCorrectly) {
    auto prev_dealer = logic_->GetDealerIndex();
    logic_->StartHand();
    EXPECT_EQ(logic_->GetDealerIndex(), player_list_.NextOccupiedSeat(prev_dealer));
    logic_->StartHand();
    EXPECT_EQ(logic_->GetDealerIndex(), player_list_.NextOccupiedSeat(*player_list_.NextOccupiedSeat(prev_dealer)));
}

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

TEST_F(GameLogicTest, CannotAdvanceStateIfNotFinished) {
    logic_->StartHand();
    EXPECT_THROW(logic_->AdvanceState(), std::runtime_error);
}

TEST_F(GameLogicTest, PlayerWinsByAllFoldAndPotIsCorrect) {
    EXPECT_CALL(*mock_table_, GetPot()).WillRepeatedly(Return(6.0));
    EXPECT_CALL(*mock_table_, CollectPot()).WillRepeatedly(Return(6.0));
    
    logic_->StartHand();
    logic_->ProcessPlayerAction({EPlayerAction::FOLD}); // Player 0 folds
    logic_->ProcessPlayerAction({EPlayerAction::FOLD}); // Player 1 folds

    auto winner = logic_->GetWinner();
    ASSERT_TRUE(winner);
    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);

    // Only seat 2 should be active
    EXPECT_EQ(winner->player_index, 2);
    EXPECT_EQ(winner->pot_amount, 6.0);
}

TEST_F(GameLogicTest, AllPlayersButOneFoldHandFinishes) {
    logic_->StartHand();
    // Fold first two, third should win
    logic_->ProcessPlayerAction({EPlayerAction::FOLD});
    logic_->ProcessPlayerAction({EPlayerAction::FOLD});
    auto winner = logic_->GetWinner();
    EXPECT_TRUE(winner);
    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);
}

TEST_F(GameLogicTest, SinglePlayerLeftEndsHand) {
    // Table with two active players
    PlayerList pl;
    pl.SitPlayerAt(MakePlayer("D"), 0);
    pl.SitPlayerAt(MakePlayer("E"), 1);
    logic_ = std::make_unique<GameLogic>(*mock_deck_, *mock_table_, pl);

    logic_->StartHand();
    logic_->ProcessPlayerAction({EPlayerAction::FOLD}); // 0 folds, 1 wins
    auto winner = logic_->GetWinner();
    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);
    EXPECT_TRUE(winner);
}

TEST_F(GameLogicTest, AllPlayersCheckAndTryToAdvanceState) {
    logic_->StartHand();
    // All check
    for (std::size_t i = 0; i < 3; ++i)
        logic_->ProcessPlayerAction({EPlayerAction::CHECK});

    EXPECT_FALSE(logic_->IsBettingRoundComplete());
    EXPECT_THROW(logic_->AdvanceState(), std::runtime_error);
    EXPECT_EQ(logic_->GetState(), ELogicState::PREFLOP);
}

TEST_F(GameLogicTest, CommunityCardsAreDealtCorrectly) {
    // Prepare a vector of 5 cards to return as community cards
    std::vector<Card> community_cards = {
        {ECardSuit::HEARTS, ECardRank::TWO},
        {ECardSuit::SPADES, ECardRank::THREE},
        {ECardSuit::DIAMONDS, ECardRank::FOUR},
        {ECardSuit::CLUBS, ECardRank::FIVE},
        {ECardSuit::HEARTS, ECardRank::SIX}
    };

    // This ensures that whenever GetCommunityCards() is called, it returns this vector
    EXPECT_CALL(*mock_table_, GetCommunityCards()).WillRepeatedly(ReturnRef(community_cards));

    logic_->StartHand();
    // Simulate betting to advance state
    for (int s = 0; s < 4; ++s) {
        for (std::size_t i = 0; i < 3; ++i) {
            if (i == 0) logic_->ProcessPlayerAction({EPlayerAction::BET, 4.0});
            else logic_->ProcessPlayerAction({EPlayerAction::CALL});
        }
        logic_->AdvanceState();
    }
    // Flop, turn, river: 3+1+1 = 5 cards
    EXPECT_EQ(mock_table_->GetCommunityCards().size(), 5);
    EXPECT_EQ(logic_->GetState(), ELogicState::SHOWDOWN);
}

TEST_F(GameLogicTest, HandCanRestartAfterFinish) {
    logic_->StartHand();
    logic_->ProcessPlayerAction({EPlayerAction::FOLD});
    logic_->ProcessPlayerAction({EPlayerAction::FOLD});
    EXPECT_TRUE(logic_->GetWinner());
    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);

    logic_->StartHand();
    EXPECT_EQ(logic_->GetState(), ELogicState::PREFLOP);
}

// Tests for All-in

TEST_F(GameLogicTest, CreateThreePots) {
    Table table(2.0, 4.0);
    Deck deck(kCardDeck, *rng_);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    player_list_.GetPlayer(1)->SetStack(200);
    player_list_.GetPlayer(2)->SetStack(300);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 90.0}); // p0
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p1
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p2
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p0

    EXPECT_TRUE(logic_->IsBettingRoundComplete());
    
    auto& pots = table.GetPots();
    EXPECT_EQ(pots.size(), 3);
    
    auto& pot_0 = table.GetPot(0);
    const std::vector<std::size_t> pp_0 {0, 1, 2};
    EXPECT_EQ(pot_0.amount, 100.0);
    EXPECT_EQ(pot_0.players, pp_0);

    auto& pot_1 = table.GetPot(1);
    const std::vector<std::size_t> pp_1 {1, 2};
    EXPECT_EQ(pot_1.amount, 200.0);
    EXPECT_EQ(pot_1.players, pp_1);

    auto& pot_2 = table.GetPot(2);
    const std::vector<std::size_t> pp_2 {2};
    EXPECT_EQ(pot_2.amount, 300.0);
    EXPECT_EQ(pot_2.players, pp_2);
}

TEST_F(GameLogicTest, ProcessTwoPotsAllIn) {
    Table table(2.0, 4.0);
    Deck deck(kCardDeck, *rng_);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 90.0}); // p0
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p1
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p2
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p0

    EXPECT_TRUE(logic_->IsBettingRoundComplete());

    // TODO: EXPECT Finish & 1 pot

}

TEST_F(GameLogicTest, ProcessTwoPotsAllIn) {
    Table table(2.0, 4.0);
    Deck deck(kCardDeck, *rng_);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    player_list_.GetPlayer(1)->SetStack(200);
    player_list_.GetPlayer(2)->SetStack(300);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 90.0}); // p0
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p1
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p2
    logic_->ProcessPlayerAction({EPlayerAction::ALL_IN}); // p0

    EXPECT_TRUE(logic_->IsBettingRoundComplete());
}*/
