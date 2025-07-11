
#include "Config.hpp"
#include "Game.hpp"

int main() {
    Game game(kScreenWidth, kScreenHeight, "SpaceInvaders");
    game.Run();

    return 0;
}