#include <gtest/gtest.h>
#include "table/PlayerList.hpp"
#include "core/Player.hpp"

// Helper
Player MakePlayer(const std::string& name, Coins_t coins = 100.0) {
    return Player(name, coins);
}

TEST(PlayerList, SitAndRemovePlayersBasic) {
    PlayerList list;

    // Sit in first available seat
    auto idx = list.SitPlayer(MakePlayer("A"));
    ASSERT_TRUE(idx);
    EXPECT_EQ(*idx, 0);
    EXPECT_TRUE(list.GetPlayer(0));
    EXPECT_EQ(list.GetPlayer(0)->GetName(), "A");

    // Sit next
    auto idx2 = list.SitPlayer(MakePlayer("B"));
    ASSERT_TRUE(idx2);
    EXPECT_EQ(*idx2, 1);
    EXPECT_TRUE(list.GetPlayer(1));
    EXPECT_EQ(list.GetPlayer(1)->GetName(), "B");

    // Remove player
    EXPECT_TRUE(list.RemovePlayer(0));
    EXPECT_FALSE(list.GetPlayer(0));
    EXPECT_FALSE(list.RemovePlayer(0)); // Removing empty
    EXPECT_FALSE(list.RemovePlayer(10)); // Out of bounds
}

TEST(PlayerList, SitPlayerAtWorksAndRejectsInvalid) {
    PlayerList list;
    EXPECT_TRUE(list.SitPlayerAt(MakePlayer("A"), 5));
    EXPECT_FALSE(list.SitPlayerAt(MakePlayer("B"), 5)); // Already occupied
    EXPECT_FALSE(list.SitPlayerAt(MakePlayer("C"), 10)); // Out of bounds
    EXPECT_TRUE(list.GetPlayer(5));
    EXPECT_EQ(list.GetPlayer(5)->GetName(), "A");
}

TEST(PlayerList, FindEmptySeatReturnsLowestIndex) {
    PlayerList list;
    EXPECT_EQ(list.FindEmptySeat(), 0);
    list.SitPlayerAt(MakePlayer("A"), 0);
    EXPECT_EQ(list.FindEmptySeat(), 1);
    for (std::size_t i = 1; i < PlayerList::kMaxPlayers; ++i)
        list.SitPlayerAt(MakePlayer("P" + std::to_string(i)), i);
    EXPECT_FALSE(list.FindEmptySeat());
}

TEST(PlayerList, NextEmptySeatWrapsCorrectly) {
    PlayerList list;
    list.SitPlayerAt(MakePlayer("A"), 0);
    list.SitPlayerAt(MakePlayer("B"), 5);
    // Next empty after 9 is 1
    EXPECT_EQ(list.NextEmptySeat(9), 1);
    // Fill 1, now next after 9 is 2
    list.SitPlayerAt(MakePlayer("C"), 1);
    EXPECT_EQ(list.NextEmptySeat(9), 2);
    // Fill all
    for (std::size_t i = 2; i < PlayerList::kMaxPlayers; ++i)
        if (i != 5) list.SitPlayerAt(MakePlayer("F" + std::to_string(i)), i);
    EXPECT_FALSE(list.NextEmptySeat(5));
}

TEST(PlayerList, NextOccupiedAndActiveSeatWrapsAndSkipsEmpty) {
    PlayerList list;
    list.SitPlayerAt(MakePlayer("A"), 0);
    list.SitPlayerAt(MakePlayer("B"), 4);
    list.SitPlayerAt(MakePlayer("C"), 8);

    // Next occupied from 0 is 4
    EXPECT_EQ(list.NextOccupiedSeat(0), 4);
    // Next occupied from 4 is 8
    EXPECT_EQ(list.NextOccupiedSeat(4), 8);
    // Next occupied from 8 is 0
    EXPECT_EQ(list.NextOccupiedSeat(8), 0);

    // All are active by default (no fold)
    EXPECT_EQ(list.NextActiveSeat(0), 4);
    // Simula que C (asiento 8) está fold
    list.GetSession(8).SetFold(true);
    EXPECT_EQ(list.NextActiveSeat(4), 0); // next active from 4 is 0
    EXPECT_EQ(list.NextActiveSeat(8), 0); // next active from 8 is 0
    // Si sólo uno activo, saltará a sí mismo en wrap
}

TEST(PlayerList, LastOccupiedSeatReturnsHighestIndex) {
    PlayerList list;
    EXPECT_FALSE(list.LastOccupiedSeat()); // Empty

    list.SitPlayerAt(MakePlayer("A"), 3);
    EXPECT_EQ(list.LastOccupiedSeat(), 3);

    list.SitPlayerAt(MakePlayer("B"), 7);
    EXPECT_EQ(list.LastOccupiedSeat(), 7);

    list.SitPlayerAt(MakePlayer("C"), 9);
    EXPECT_EQ(list.LastOccupiedSeat(), 9);

    list.RemovePlayer(9);
    EXPECT_EQ(list.LastOccupiedSeat(), 7);
}

TEST(PlayerList, FindOnlyActiveSeatWorks) {
    PlayerList list;
    // No active
    EXPECT_FALSE(list.FindFirstActiveSeat());

    // One active
    list.SitPlayerAt(MakePlayer("A"), 2);
    EXPECT_EQ(list.FindFirstActiveSeat(), 2);

    // Two active
    list.SitPlayerAt(MakePlayer("B"), 5);
    EXPECT_EQ(list.FindFirstActiveSeat(), 2);

    // One in fold
    list.GetSession(2).SetFold(true);
    EXPECT_EQ(list.FindFirstActiveSeat(), 5);

    // All fold
    list.GetSession(5).SetFold(true);
    EXPECT_FALSE(list.FindFirstActiveSeat());
}

TEST(PlayerList, GetActiveSeatIndicesAndCountActiveSeats) {
    PlayerList list;
    EXPECT_TRUE(list.GetActiveSeatIndices().empty());
    EXPECT_EQ(list.CountActiveSeats(), 0);

    list.SitPlayerAt(MakePlayer("A"), 0);
    list.SitPlayerAt(MakePlayer("B"), 1);
    list.SitPlayerAt(MakePlayer("C"), 2);

    EXPECT_EQ(list.GetActiveSeatIndices(), (std::vector<std::size_t>{0,1,2}));
    EXPECT_EQ(list.CountActiveSeats(), 3);

    // Fold one
    list.GetSession(1).SetFold(true);
    EXPECT_EQ(list.GetActiveSeatIndices(), (std::vector<std::size_t>{0,2}));
    EXPECT_EQ(list.CountActiveSeats(), 2);
}

TEST(PlayerList, GetOccupiedSeatIndicesAndCountOccupiedSeats) {
    PlayerList list;
    list.SitPlayerAt(MakePlayer("A"), 2);
    list.SitPlayerAt(MakePlayer("B"), 4);
    list.SitPlayerAt(MakePlayer("C"), 8);

    EXPECT_EQ(list.GetOccupiedSeatIndices(), (std::vector<std::size_t>{2,4,8}));
    EXPECT_EQ(list.CountOccupiedSeats(), 3);

    // Remove one
    list.RemovePlayer(4);
    EXPECT_EQ(list.GetOccupiedSeatIndices(), (std::vector<std::size_t>{2,8}));
    EXPECT_EQ(list.CountOccupiedSeats(), 2);
}

TEST(PlayerList, GetPlayerAndSessionAccess) {
    PlayerList list;
    auto idx = list.SitPlayer(MakePlayer("Javier", 777));
    ASSERT_TRUE(idx);
    auto& player_opt = list.GetPlayer(*idx);
    ASSERT_TRUE(player_opt);
    EXPECT_EQ(player_opt->GetName(), "Javier");
    EXPECT_EQ(player_opt->GetStack(), 777);

    // Acceso a sesión
    PlayerSession& session = list.GetSession(*idx);
    session.SetFold(true);
    EXPECT_TRUE(session.IsFold());
}

TEST(PlayerList, ResetSessionsClearsAllSessionState) {
    PlayerList list;
    for (std::size_t i = 0; i < 3; ++i)
        list.SitPlayerAt(MakePlayer("P" + std::to_string(i)), i);

    // Cambia estado de sesiones
    for (std::size_t i = 0; i < 3; ++i)
        list.GetSession(i).SetFold(true);

    // Resetea todas
    list.ResetSessions();
    for (std::size_t i = 0; i < 3; ++i)
        EXPECT_FALSE(list.GetSession(i).IsFold());
}
