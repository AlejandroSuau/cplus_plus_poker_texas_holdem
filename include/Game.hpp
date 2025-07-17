#pragma once

#include <raylib.h>

#include "Config.hpp"

#include <string>

class Game {
public:
    Game(int width, int height, std::string title);
    void Run();

private:
    int width_, height_;
    std::string title_;

    void Init();
    void Update();
    void Draw();
};
