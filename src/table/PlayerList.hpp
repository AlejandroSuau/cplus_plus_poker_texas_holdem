#pragma once

#include "Config.hpp"

#include "core/Player.hpp"

#include <array>
#include <optional>

// TODO: PlayerList or handle players other way.

class PlayerList {
public:
    constexpr static std::size_t kMaxPlayers = 10;
    using Players_t = std::array<std::optional<Player>, kMaxPlayers>;
    
    PlayerList();

    void Add(Player player);
    void Remove(std::size_t index);
    void Clear();

    Player& NextAvailable(std::size_t index);
    std::size_t NextAvailableIndex(std::size_t index);
    Players_t& GetPlayers();

private:
    Players_t players_;
    
};
