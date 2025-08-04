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

#include <unordered_set>
#include <algorithm>

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

// --- Fixture ---
class GameLogicTest : public ::testing::Test {
protected:
    std::unique_ptr<testing::NiceMock<MockDeck>> mock_deck_;
    std::unique_ptr<testing::NiceMock<MockTable>> mock_table_;
    PlayerList player_list_;
    std::unique_ptr<IRandomProvider> rng_;
    std::unique_ptr<GameLogic> logic_;

    std::vector<Card> default_community_cards_;
    std::vector<Pot> default_pots_;

    void SetUp() override {
        mock_deck_ = std::make_unique<testing::NiceMock<MockDeck>>();
        mock_table_ = std::make_unique<testing::NiceMock<MockTable>>();
        rng_ = std::make_unique<StdRandomProvider>();

        // Fill seats 0, 1, 2 with players
        player_list_.SitPlayerAt(MakePlayer("A"), 0);
        player_list_.SitPlayerAt(MakePlayer("B"), 1);
        player_list_.SitPlayerAt(MakePlayer("C"), 2);
        player_list_.SitPlayerAt(MakePlayer("D"), 3);
        player_list_.SitPlayerAt(MakePlayer("E"), 3);

        // Always return the same card for simplicity in mocks
        std::optional<Card> fake_card = Card{ECardSuit::HEARTS, ECardRank::ACE};
        EXPECT_CALL(*mock_deck_, Draw()).WillRepeatedly(Return(fake_card));
        EXPECT_CALL(*mock_table_, GetBlindSmall()).WillRepeatedly(Return(2.0));
        EXPECT_CALL(*mock_table_, GetBlindBig()).WillRepeatedly(Return(4.0));

        ON_CALL(*mock_table_, GetCommunityCards())
            .WillByDefault(ReturnRef(default_community_cards_));
        ON_CALL(*mock_table_, GetPots())
            .WillByDefault(ReturnRef(default_pots_));

        logic_ = std::make_unique<GameLogic>(*mock_deck_, *mock_table_, player_list_);
    }

    static Player MakePlayer(const std::string& name, Coins_t chips = 100.0) {
        return Player(name, chips);
    }
};

TEST_F(GameLogicTest, BlindArePaidOnStartingHand) {
    const Coins_t sb = 2.0;
    const Coins_t bb = 4.0;
    Deck deck(kCardDeck, *rng_);
    Table table(sb, bb);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    auto player_1_stack_init = player_list_.GetPlayer(1).GetStack();
    auto player_2_stack_init = player_list_.GetPlayer(2).GetStack();

    logic_->StartHand();

    EXPECT_EQ(logic_->GetState(), ELogicState::PREFLOP);
    
    // Player 1 substracted bb
    auto& player_1 = player_list_.GetPlayer(1);
    EXPECT_DOUBLE_EQ(player_1.GetStack(), player_1_stack_init - sb);

    // Player 2 substracted bb
    auto& player_2 = player_list_.GetPlayer(2);
    EXPECT_DOUBLE_EQ(player_2.GetStack(), player_2_stack_init - bb);

    // Pot state
    const auto& pots = table.GetPots();
    EXPECT_EQ(pots.size(), 1);
    EXPECT_DOUBLE_EQ(pots[0].amount, sb + bb);

    auto& pot_players = pots[0].players;
    const std::unordered_set<std::size_t> expected_players {1, 2};
    EXPECT_EQ(pot_players, expected_players);
}

TEST_F(GameLogicTest, TransitionBetweenAllStates) {
    player_list_.ClearPlayers();
    player_list_.SitPlayerAt(MakePlayer("A"), 0);
    player_list_.SitPlayerAt(MakePlayer("B"), 1);

    logic_->StartHand();

    EXPECT_EQ(logic_->GetDealerIndex(), 0);
    EXPECT_EQ(logic_->GetCurrentPlayerIndex(), 1);

    const std::array<ELogicState, 4> expected_states {
        {ELogicState::FLOP, ELogicState::TURN, ELogicState::RIVER, ELogicState::SHOWDOWN}
    };
    for (std::size_t i = 0; i < expected_states.size(); ++i) {
        logic_->ProcessPlayerAction({EPlayerAction::BET, 10.0});
        logic_->ProcessPlayerAction({EPlayerAction::BET, 10.0});
        
        EXPECT_TRUE(logic_->IsBettingRoundComplete());
        
        logic_->AdvanceState();
        
        EXPECT_EQ(logic_->GetState(), expected_states[i]);
    }

    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);
}

TEST_F(GameLogicTest, NoMorePlayersWithChipsAfterAllIn) {
    player_list_.ClearPlayers();
    player_list_.SitPlayerAt(MakePlayer("A", 100), 0); // dealer
    player_list_.SitPlayerAt(MakePlayer("B", 100), 1); // small blind
    player_list_.SitPlayerAt(MakePlayer("C", 100), 2); // big blind
    player_list_.SitPlayerAt(MakePlayer("D", 100), 3); 

    const Coins_t sb = 2.0;
    const Coins_t bb = 4.0;
    Deck deck(kCardDeck, *rng_);
    Table table(sb, bb);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // A (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // B (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // C (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // D (all-in)

    // Check Pot
    const std::unordered_set<std::size_t> expected_pot_players {0, 1, 2, 3};
    const auto& pots = table.GetPots();
    EXPECT_EQ(pots.size(), 1);
    EXPECT_EQ(pots[0].players, expected_pot_players);
    EXPECT_DOUBLE_EQ(pots[0].amount, 100.0 * 4.0);

    // Check Logic State
    EXPECT_EQ(logic_->GetState(), ELogicState::SHOWDOWN);
    EXPECT_EQ(table.GetCommunityCards().size(), 5);

    for (const auto i : player_list_.GetActiveSeatIndices()) {
        const auto& player = player_list_.GetPlayer(i);
        EXPECT_EQ(player.GetStack(), 0.0);
    }

    logic_->AdvanceState();

    // Check hand finished and prize payment
    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);
    EXPECT_DOUBLE_EQ(pots[0].amount, 0.0);

    const auto players = player_list_.GetActiveSeatIndices();
    EXPECT_TRUE(std::any_of(players.cbegin(), players.cend(), [&](const auto i) {
        return (player_list_.GetPlayer(i).GetStack() > 0.0);
    }));
}

TEST_F(GameLogicTest, OnePlayerWithChipsAfterAllIn) {
    player_list_.ClearPlayers();
    player_list_.SitPlayerAt(MakePlayer("A", 25), 0); // dealer
    player_list_.SitPlayerAt(MakePlayer("B", 100), 1); // small blind
    player_list_.SitPlayerAt(MakePlayer("C", 100), 2); // big blind
    player_list_.SitPlayerAt(MakePlayer("D", 100), 3); 

    const Coins_t sb = 2.0;
    const Coins_t bb = 4.0;
    Deck deck(kCardDeck, *rng_);
    Table table(sb, bb);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 25.0}); // A (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 25.0}); // B
    logic_->ProcessPlayerAction({EPlayerAction::BET, 25.0}); // C
    logic_->ProcessPlayerAction({EPlayerAction::BET, 25.0}); // D

    
}

TEST_F(GameLogicTest, LogicEdgeCase1) {
    /*player_list_.ClearPlayers();
    player_list_.SitPlayerAt(MakePlayer("A", 100), 0); // dealer
    player_list_.SitPlayerAt(MakePlayer("B", 150), 1); // small blind
    player_list_.SitPlayerAt(MakePlayer("C", 150), 2); // big blind

    const Coins_t sb = 2.0;
    const Coins_t bb = 4.0;
    Deck deck(kCardDeck, *rng_);
    Table table(sb, bb);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // A (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // B
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // C
    // Pot 0 => 0, 1, 2; 300 chips

    logic_->AdvanceState();
    
    EXPECT_EQ(logic_->GetState(), ELogicState::FLOP);

    EXPECT_EQ(table.GetCommunityCards().size(), 3);

    // A is not playing anymore. Waiting for hand to finish.
    logic_->ProcessPlayerAction({EPlayerAction::BET, 25.0}); // B
    logic_->ProcessPlayerAction({EPlayerAction::BET, 50.0}); // C (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::FOLD}); // B
    // Pot 1 => 1, 2; 75 chips

    EXPECT_EQ(logic_->GetState(), ELogicState::SHOWDOWN);
    EXPECT_EQ(table.GetCommunityCards().size(), 5);
    
    // All players must have ranks here.
    const auto& pots = table.GetPots();
    EXPECT_EQ(pots.size(), 1);

    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);

    // Prizes are already given here.

    // Player C wins pot 2. B loses last 25 chips before fold.
    // Who wins Pot 1? (0, 1, 2)?*/
}

TEST_F(GameLogicTest, LogicEdgeCase2) {
    /*player_list_.ClearPlayers();
    player_list_.SitPlayerAt(MakePlayer("A", 100), 0); // dealer
    player_list_.SitPlayerAt(MakePlayer("B", 150), 1); // small blind
    player_list_.SitPlayerAt(MakePlayer("C", 150), 2); // big blind
    
    const Coins_t sb = 2.0;
    const Coins_t bb = 4.0;
    Deck deck(kCardDeck, *rng_);
    Table table(sb, bb);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // A (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // B
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // C
    // Pot 0 => 0, 1, 2; 300 chips

    logic_->AdvanceState();
    
    // A is not playing anymore. Waiting for hand to finish.
    logic_->ProcessPlayerAction({EPlayerAction::BET, 25.0}); // B
    logic_->ProcessPlayerAction({EPlayerAction::BET, 50.0}); // C (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 50.0}); // B (all-in)
    
    logic_->AdvanceState();
    
    // Check pots
    const auto& pots = table.GetPots();
    EXPECT_EQ(pots.size(), 2);
    
    const std::unordered_set<std::size_t> expected_players_pot0 {0, 1, 2};
    EXPECT_EQ(pots[0].players, expected_players_pot0);

    const std::unordered_set<std::size_t> expected_players_pot1 {1, 2};
    EXPECT_EQ(pots[1].players, expected_players_pot1);

    EXPECT_EQ(logic_->GetState(), ELogicState::SHOWDOWN);

    // Check that all players involved in a pot has a rank != 0

    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);

    // Who wins Pot 1? (0, 1, 2)?
    // Who wins Pot 2? (1, 2)?*/
}

TEST_F(GameLogicTest, LogicEdgeCase3) {
    /*player_list_.ClearPlayers();
    player_list_.SitPlayerAt(MakePlayer("A", 150), 0); // dealer
    player_list_.SitPlayerAt(MakePlayer("B", 120), 1); // small blind
    player_list_.SitPlayerAt(MakePlayer("C", 100), 2); // big blind
    
    const Coins_t sb = 2.0;
    const Coins_t bb = 4.0;
    Deck deck(kCardDeck, *rng_);
    Table table(sb, bb);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 150.0}); // A (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 120.0}); // B (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // C (all-in)
    // Pot 0 => 0, 1, 2; 300 chips
    // Pot 1 => 0, 1; 240 chips
    // Pot 2 => 0; 30 chips // Or not pot 2.

    EXPECT_EQ(logic_->GetState(), ELogicState::SHOWDOWN);

    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);
    
    // Check pots
    const auto& pots = table.GetPots();
    EXPECT_EQ(pots.size(), 3);
    
    const std::unordered_set<std::size_t> expected_players_pot0 {0, 1, 2};
    EXPECT_EQ(pots[0].players, expected_players_pot0);

    const std::unordered_set<std::size_t> expected_players_pot1 {0, 1};
    EXPECT_EQ(pots[1].players, expected_players_pot1);

    const std::unordered_set<std::size_t> expected_players_pot2 {0};
    EXPECT_EQ(pots[2].players, expected_players_pot2);*/
}

TEST_F(GameLogicTest, LogicEdgeCase4) {
    /*player_list_.ClearPlayers();
    player_list_.SitPlayerAt(MakePlayer("A", 150), 0); // dealer
    player_list_.SitPlayerAt(MakePlayer("B", 120), 1); // small blind
    player_list_.SitPlayerAt(MakePlayer("C", 100), 2); // big blind
    
    const Coins_t sb = 2.0;
    const Coins_t bb = 4.0;
    Deck deck(kCardDeck, *rng_);
    Table table(sb, bb);
    logic_ = std::make_unique<GameLogic>(deck, table, player_list_);

    logic_->StartHand();

    logic_->ProcessPlayerAction({EPlayerAction::BET, 80.0}); // A
    logic_->ProcessPlayerAction({EPlayerAction::BET, 80.0}); // B
    logic_->ProcessPlayerAction({EPlayerAction::BET, 100.0}); // C (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 150.0}); // A (all-in)
    logic_->ProcessPlayerAction({EPlayerAction::BET, 120.0}); // B (all-in)

    // Pot 0 => 0, 1, 2; 300 chips
    // Pot 1 => 0, 1; 240 chips
    // Pot 2 => 0; 30 chips // Or not pot 2.

    EXPECT_EQ(logic_->GetState(), ELogicState::SHOWDOWN);

    logic_->AdvanceState();

    EXPECT_EQ(logic_->GetState(), ELogicState::HAND_FINISHED);
    
    // Check pots
    const auto& pots = table.GetPots();
    EXPECT_EQ(pots.size(), 3);
    
    const std::unordered_set<std::size_t> expected_players_pot0 {0, 1, 2};
    EXPECT_EQ(pots[0].players, expected_players_pot0);

    const std::unordered_set<std::size_t> expected_players_pot1 {0, 1};
    EXPECT_EQ(pots[1].players, expected_players_pot1);

    const std::unordered_set<std::size_t> expected_players_pot2 {0};
    EXPECT_EQ(pots[2].players, expected_players_pot2);*/
}