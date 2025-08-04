#include "table/PlayerList.hpp"

#include <ranges>
#include <algorithm>

PlayerList::PlayerList() {
    ClearPlayers();
}

void PlayerList::ClearPlayers() {
    for (std::size_t i = 0; i < seats_.size(); ++i) {
        RemovePlayer(i);
    }
}

std::optional<std::size_t> PlayerList::SitPlayer(Player&& player) {
    for (std::size_t i = 0; i < seats_.size(); ++i) {
        if (seats_[i].player) continue;
        
        seats_[i].player = std::move(player);
        seats_[i].session = {};
        return i;
    }

    return std::nullopt;
}

bool PlayerList::SitPlayerAt(Player&& player, std::size_t seat_index) {
    if (seat_index >= kMaxPlayers || seats_[seat_index].player) return false;
    
    seats_[seat_index].player = std::move(player);
    seats_[seat_index].session = {};
    return true;
}

bool PlayerList::RemovePlayer(std::size_t seat_index) {
    if (seat_index >= kMaxPlayers || !seats_[seat_index].player) return false;
    
    seats_[seat_index].player = std::nullopt;
    seats_[seat_index].session = {};
    return true;
}

std::optional<std::size_t> PlayerList::FindEmptySeat() const {
    for (std::size_t i = 0; i < seats_.size(); ++i)
        if (!seats_[i].player) return i;
    return std::nullopt;
}

std::optional<std::size_t> PlayerList::NextOccupiedSeat(std::size_t from) const {
    std::size_t idx = from;
    for (std::size_t i = 0; i < kMaxPlayers; ++i) {
        idx = (idx + 1) % kMaxPlayers;
        if (seats_[idx].player) return idx;
    }
    return std::nullopt;
}

std::optional<std::size_t> PlayerList::NextActiveSeat(std::size_t from) const {
    std::size_t idx = from;
    for (std::size_t i = 0; i < kMaxPlayers; ++i) {
        idx = (idx + 1) % kMaxPlayers;
        if (seats_[idx].player && !seats_[idx].session.IsFold()) return idx;
    }
    return std::nullopt;
}

std::optional<std::size_t> PlayerList::NextEmptySeat(std::size_t from) const {
    std::size_t idx = from;
    for (std::size_t i = 0; i < kMaxPlayers; ++i) {
        idx = (idx + 1) % kMaxPlayers;
        if (!seats_[idx].player) return idx;
    }
    return std::nullopt;
}

std::size_t PlayerList::CountAllInPlayers() const {
    const auto active_seat_indices = GetActiveSeatIndices();
    return std::count_if(active_seat_indices.begin(), active_seat_indices.end(), [&](const auto i) {
        return (GetSession(i).IsAllIn());
    });
}

Coins_t PlayerList::GetMinLastBet() const {
    auto active_seat_indices  = GetActiveSeatIndices();
    auto filtered_stacks = active_seat_indices 
        | std::views::transform([&](const auto i) {
            return GetPlayer(i).GetStack();
        });

    auto it = std::ranges::min_element(filtered_stacks);
    return *it;
}

std::optional<std::size_t> PlayerList::LastOccupiedSeat() const {
    for (std::size_t i = kMaxPlayers; i > 0; --i) {
        std::size_t idx = i - 1;
        if (seats_[idx].player) {
            return idx;
        }
    }
    return std::nullopt;
}

std::optional<std::size_t> PlayerList::FindFirstActiveSeat() const {
    
    for (std::size_t i = 0; i < kMaxPlayers; ++i) {
        if (seats_[i].player && !seats_[i].session.IsFold()) {
            return i;
        }
    }
    return std::nullopt;
}

std::vector<std::size_t> PlayerList::GetOccupiedSeatIndices() const {
    std::vector<std::size_t> indices;
    for (std::size_t i = 0; i < seats_.size(); ++i) {
        if (seats_[i].player) indices.push_back(i);
    }
    return indices;
}

std::vector<std::size_t> PlayerList::GetActiveSeatIndices() const {
    std::vector<std::size_t> indices;
    for (std::size_t i = 0; i < seats_.size(); ++i)
        if (seats_[i].player && !seats_[i].session.IsFold()) indices.push_back(i);
    return indices;
}

std::size_t PlayerList::CountOccupiedSeats() const {
    return std::ranges::count_if(
        seats_,
        [&](const auto& s) { return (s.player.has_value()); });
}

std::size_t PlayerList::CountActiveSeats() const {
    return std::ranges::count_if(
        seats_,
        [&](const auto& s) { return (s.player.has_value() && !s.session.IsFold()); });
}

Player& PlayerList::GetPlayer(std::size_t seat_index) {
    return *seats_[seat_index].player;
}

const Player& PlayerList::GetPlayer(std::size_t seat_index) const {
    return *seats_[seat_index].player;
}

PlayerSession& PlayerList::GetSession(std::size_t seat_index) {
    return seats_[seat_index].session;
}

const PlayerSession& PlayerList::GetSession(std::size_t seat_index) const {
    return seats_[seat_index].session;
}

PlayerList::Seat& PlayerList::GetSeat(std::size_t seat_index) {
    return seats_[seat_index];
}
const PlayerList::Seat& PlayerList::GetSeat(std::size_t seat_index) const {
    return seats_[seat_index];
}

void PlayerList::ResetSessions() {
    for (auto& seat : seats_) {
        seat.session = {};
    }
}