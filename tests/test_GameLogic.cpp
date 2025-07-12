#include <gtest/gtest.h>

#include "Config.hpp"

#include "core/Card.hpp"
#include "core/Deck.hpp"
#include "core/Player.hpp"

#include "game_logic/GameLogic.hpp"


TEST(GameLogic, StartHand) {
    Deck deck {kCardDeck};
    Table table(kBlindSmall, kBlindBig);
    Players players { {"Player 1", 50.0}, {"Player 2", 75.0}, {"Player 3", 25.0}};

    for (auto& player : players) { EXPECT_TRUE(player.GetHand().IsEmpty()); }

    GameLogic logic(deck, table, players);
    logic.StartHand();

    EXPECT_EQ(logic.GetState(), EState::PREFLOP);

    const auto small_blind = table.GetBlindSmall();
    const auto big_blind = table.GetBlindBig();
    
    EXPECT_EQ(table.GetPot(), small_blind + big_blind);
    EXPECT_EQ(players[1].GetCoins(), 75.0 - small_blind);
    EXPECT_EQ(players[2].GetCoins(), 25.0 - big_blind);

    for (auto& player : players) { EXPECT_TRUE(player.GetHand().IsFull()); }
}
