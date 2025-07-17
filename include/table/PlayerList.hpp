#pragma once

#include <array>
#include <optional>
#include <vector>
#include <string>

#include "core/Player.hpp"
#include "table/PlayerSession.hpp"

// Note: Active Seat
//       Seat has player; Player is not fold.
//       Occupied Seat
//       Seat has player;

class PlayerList {
public:
    static constexpr std::size_t kMaxPlayers = 10;

    struct Seat {
        std::optional<Player> player;
        PlayerSession session;
    };

    PlayerList();

    std::optional<std::size_t> SitPlayer(Player&& player);
    bool SitPlayerAt(Player&& player, std::size_t seat_index);
    bool RemovePlayer(std::size_t seat_index);
    
    std::optional<std::size_t> NextOccupiedSeat(std::size_t from) const;
    std::optional<std::size_t> NextActiveSeat(std::size_t from) const;
    std::optional<std::size_t> NextEmptySeat(std::size_t from) const;
    std::optional<std::size_t> LastOccupiedSeat() const;
    std::optional<std::size_t> FindFirstActiveSeat() const;

    std::optional<std::size_t> FindEmptySeat() const;
    std::vector<std::size_t> GetActiveSeatIndices() const;
    std::size_t CountActiveSeats() const;
    std::vector<std::size_t> GetOccupiedSeatIndices() const;
    std::size_t CountOccupiedSeats() const;

    std::optional<Player>& GetPlayer(std::size_t seat_index);
    const std::optional<Player>& GetPlayer(std::size_t seat_index) const;

    PlayerSession& GetSession(std::size_t seat_index);
    const PlayerSession& GetSession(std::size_t seat_index) const;

    Seat& GetSeat(std::size_t seat_index);
    const Seat& GetSeat(std::size_t seat_index) const;

    // Iteratos for range
    auto begin() { return seats_.begin(); }
    auto end()   { return seats_.end();   }
    auto begin() const { return seats_.begin(); }
    auto end()   const { return seats_.end();   }

    void ResetSessions();

private:
    std::array<Seat, kMaxPlayers> seats_;
};