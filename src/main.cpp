
#include "Config.hpp"
#include "Game.hpp"

int main() {
    Game game(kScreenWidth, kScreenHeight, "PokerGame");
    game.Run();

    return 0;
}